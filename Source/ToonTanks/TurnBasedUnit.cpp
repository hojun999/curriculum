// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedUnit.h"
#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"
#include "GridManager.h"
#include "TimerManager.h"


// Sets default values
ATurnBasedUnit::ATurnBasedUnit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATurnBasedUnit::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작되면 월드의 TurnManager를 찾아 저장
	TurnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATurnManager::StaticClass()));

	if (TurnManager) {
		TurnManager->RegisterUnit(this);
	}
}

void ATurnBasedUnit::OnTurnStarted_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn started."), *GetName());
	bIsMyTurn = true;	// 내 턴이라고 상태만 변경한 후 대기
	bIsExecutingActions = false;

	// 턴 시작 시 행동력 및 액션 큐 초기화
	CurrentActionPoints = MaxActionPoints;
	ActionQueue.Empty();

	if (!bIsAlly) {
		// 적 유닛이라면 AI 실행
		ExecuteEnemyAI();
	}

	// TODO 1 : 행동을 위한 UI활성화나 상태 변경 로직 추가
	// 행동이 끝나면 반드시 TurnManager에게 알려줘야됨
	// ex : Atack()이 끝나면 TurnManager->OnUnitActionFinished(); 호출
	// 지금은 즉시 행동을 마쳤다고 가정
}

void ATurnBasedUnit::OnTurnEnded_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn ended."), *GetName());
	bIsMyTurn = false;	// 내 턴이 끝났음을 표시
}

void ATurnBasedUnit::ProcessNextAction()
{
	// 실행 중이 아니거나 턴이 아니면 종료
	if (!bIsMyTurn || !bIsExecutingActions) {
		return;
	}
	FIntPoint TargetCoordinate = CurrentGridCoordinate;

	EUnitAction NextAction;
	// 큐에서 다음 행동 꺼내옴
	if (!ActionQueue.Dequeue(NextAction)) {
		// 큐가 비어있다면, 모든 행동이 끝난 것으로 턴 종료
		EndTurn();
		return;
	}

	// 큐에 행동이 남아있다면, 하나를 실행
	bool bActionSuccess = false;

	switch (NextAction) {
	case EUnitAction::MoveUp:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Up ---"));
		TargetCoordinate += FIntPoint(1, 0);
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::MoveDown:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Down ---"));
		TargetCoordinate += FIntPoint(-1, 0);
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::MoveLeft:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Left ---"));
		TargetCoordinate += FIntPoint(0, -1);
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::MoveRight:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Right ---"));
		TargetCoordinate += FIntPoint(0, 1);
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::Attack:
		break;
		// TODO: 1. 가장 가까운 적 유닛 찾기
		// 2. 최대사거리보다 멀면 최대사거리의 gird에 공격
		// 3. 최대사거리보다 가까우면 해당 유닛 공격
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ATurnBasedUnit::ProcessNextAction,
		1.5f,
		false
	);
}

void ATurnBasedUnit::EndTurn()
{
	if (!bIsMyTurn) {
		UE_LOG(LogTemp, Warning, TEXT("--- NOT MY TURN ---"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("--- %s's Action Queue Empty. Ending Turn. ---"), *GetName());

	// 실행 잠금 해제
	bIsExecutingActions = false;
	// 남은 행동력이 있어도 턴을 종료했으므로 0으로 만듦
	CurrentActionPoints = 0;

	// TurnManager에게 턴이 끝났음을 보고
	if (TurnManager) {
		TurnManager->OnUnitActionFinished();
	}
}

// 입력 바인딩용 래퍼 함수들
void ATurnBasedUnit::AddMoveUpAction() { ActionQueue.Enqueue(EUnitAction::MoveUp); CommonActionsOnBinding(); }
void ATurnBasedUnit::AddMoveDownAction() { ActionQueue.Enqueue(EUnitAction::MoveDown); CommonActionsOnBinding(); }
void ATurnBasedUnit::AddMoveLeftAction() { ActionQueue.Enqueue(EUnitAction::MoveLeft); CommonActionsOnBinding(); }
void ATurnBasedUnit::AddMoveRightAction() { ActionQueue.Enqueue(EUnitAction::MoveRight); CommonActionsOnBinding(); }
void ATurnBasedUnit::AddAttack() { ActionQueue.Enqueue(EUnitAction::Attack); CommonActionsOnBinding(); }

void ATurnBasedUnit::CommonActionsOnBinding()
{
	CurrentActionPoints--;
	UE_LOG(LogTemp, Warning, TEXT("--- CurrentActionPoints: %d ---"), CurrentActionPoints);
}

void ATurnBasedUnit::Initialize(FIntPoint StartCoordinate) {
	CurrentGridCoordinate = StartCoordinate;
}

void ATurnBasedUnit::ExecuteActionQueue()
{
	// 턴이 아니거나 이미 실행 중이면 무시
	if (!bIsMyTurn || bIsExecutingActions) {
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("--- Executing Action Queue ---"));

	// 잠금을 설정하여 턴이 끝날 때까지 추가 입력 방지
	bIsExecutingActions = true;

	// 큐 처리 시작
	ProcessNextAction();
}

bool ATurnBasedUnit::AttemptMove(FIntPoint TargetCoordinate)
{
	// 내 턴일 때만 이동 가능
	if (!bIsMyTurn)
		return false;

	// GridManager 유효성 검사
	AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (!GridManager)
		return false;

	FVector TargetWorldLocation;

	// GridManager에게 목표 좌표 이동 가능 여부 확인
	if (GridManager->GetTileWorldLocation(TargetCoordinate, TargetWorldLocation)) {
		// 이동 가능한 경우 해당 위치로 액터 이동
		UE_LOG(LogTemp, Log, TEXT("%s moves from %s to %s"), *GetName(), *CurrentGridCoordinate.ToString(), *TargetCoordinate.ToString());


		// 수치 조정으로 부드럽게 이동
		TargetWorldLocation.Z += 35.0f;
		SetActorLocation(TargetWorldLocation);

		// 나의 현재 그리드 좌표 갱신
		CurrentGridCoordinate = TargetCoordinate;

		return true;
	}
	
	return false;
}

void ATurnBasedUnit::ExecuteEnemyAI()
{
	// 즉시 행동하게 하려면 아래 타이머 삭제
	FTimerHandle AiActionTimer;
	GetWorld()->GetTimerManager().SetTimer(AiActionTimer, [this]() {

		// 1. 랜덤 방향 선택 (0:상, 1:하, 2:좌, 3:우)
		int32 RandomDirection = FMath::RandRange(0, 3);
		bool bMoveSuccess = false;

		switch (RandomDirection)
		{
		case 0: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(0, 1)); break;
		case 1: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(0, -1)); break;
		case 2: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(-1, 0)); break;
		case 3: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(1, 0)); break;
		}

		if (bMoveSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("AI Unit %s moved."), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("AI Unit %s failed to move (blocked)."), *GetName());
		}

		// 2. (매우 중요) 이동에 성공하든 실패하든, AI의 행동은 끝났으므로 턴을 종료합니다.
		//    (이동 후 0.5초 정도 더 기다렸다가 턴을 넘겨야 자연스럽습니다)
		FTimerHandle EndTurnTimer;
		GetWorld()->GetTimerManager().SetTimer(
			EndTurnTimer,
			this,
			&ATurnBasedUnit::EndTurn, // EndTurn 함수가 TurnManager->OnUnitActionFinished()를 호출
			0.5f,
			false
		);

		}, 0.5f, false);
}

// Called every frame
void ATurnBasedUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
//void ATurnBasedUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

