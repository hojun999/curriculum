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

	// 폰이 컨트롤러의 회전을 자동으로 따라가지 않도록 설정
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
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

	GridManagerRef = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
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
	if (!bIsMyTurn || !bIsExecutingActions || bIsMoving) {
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
		UE_LOG(LogTemp, Warning, TEXT("--- Attack Action ---"));
		HandleAttackAction();
		break;
	}

	//FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(
	//	TimerHandle,
	//	this,
	//	&ATurnBasedUnit::ProcessNextAction,
	//	1.5f,
	//	false
	//);
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

	AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (GridManager) {
		ATile* StartTile = GridManager->GetTile(StartCoordinate);
		if (StartTile) {
			StartTile->OccupyingUnit = this;
		}
	}
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
	if (!bIsMyTurn || bIsMoving)
		return false;

	// GridManager 유효성 검사
	if (!GridManagerRef)
		return false;

	if (GridManagerRef->IsTileOccupied(TargetCoordinate)) {
		return false;
	}

	FVector TargetWorldLocation;

	// GridManager에게 목표 좌표 이동 가능 여부 확인
	if (GridManagerRef->GetTileWorldLocation(TargetCoordinate, TargetWorldLocation)) {
		// 이동 가능한 경우 해당 위치로 액터 이동
		UE_LOG(LogTemp, Log, TEXT("%s moves from %s to %s"), *GetName(), *CurrentGridCoordinate.ToString(), *TargetCoordinate.ToString());


		// 수치 조정으로 부드럽게 이동
		TargetWorldLocation.Z += 35.0f;

		// 1. 이동 방향을 그리드 좌표로 계산
		const FIntPoint DirectionVector = TargetCoordinate - CurrentGridCoordinate;

		// 2. 방향에 따라 목표 Yaw 회전값 게산 (ProcessNextAction의 방향에 맞게 설정)
		if (DirectionVector == FIntPoint(1, 0)) { // MoveUp
			SmoothMoveTargetRotation = FRotator(0.0f, 0.0f, 0.0f); // 0도
		}
		else if (DirectionVector == FIntPoint(-1, 0)) { // MoveDown
			SmoothMoveTargetRotation = FRotator(0.0f, 180.0f, 0.0f);
		}
		else if (DirectionVector == FIntPoint(0, -1)) { // MoveLeft
			SmoothMoveTargetRotation = FRotator(0.0f, -90.0f, 0.0f);
		}
		else if (DirectionVector == FIntPoint(0, 1)) { // MoveRight
			SmoothMoveTargetRotation = FRotator(0.0f, 90.0f, 0.0f);
		}

		// 3. 이동 및 회전 시작
		SmoothMoveTargetLocation = TargetWorldLocation;
		bIsMoving = true; // 이동 시작

		GridManagerRef->UpdateTileOccupancy(CurrentGridCoordinate, TargetCoordinate, this);

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
		case 0: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(1, 0)); break; // up
		case 1: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(-1, 0)); break; // down
		case 2: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(0, -1)); break; // left
		case 3: bMoveSuccess = AttemptMove(CurrentGridCoordinate + FIntPoint(0, 1)); break; // right
		}

		if (bMoveSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("AI Unit %s moved."), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("AI Unit %s failed to move (blocked)."), *GetName());
			EndTurn();
		}

		}, 0.5f, false);
}

// Called every frame
void ATurnBasedUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 부드러운 이동 중인지 확인
	if (bIsMoving) {
		FVector CurrentLocation = GetActorLocation();
		FRotator CurrentRotation = GetActorRotation();

		// ----- 부드러운 회전

		// RInterpTo를 사용해 현재 회전에서 목표 회전으로 부드럽게 보간
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,
			SmoothMoveTargetRotation,
			DeltaTime,
			RotationSpeed
		);

		SetActorRotation(NewRotation);

		// ----- 부드러운 이동
		FVector NewLocation = FMath::VInterpTo(
			CurrentLocation, // 현재 위치
			SmoothMoveTargetLocation, // 목표 위치
			DeltaTime, // 델타 타임
			MoveSpeed / 100.0f // 보간 속도
		);

		SetActorLocation(NewLocation); // 계산된 위치값 적용

		// 이동 완료 확인
		if (FVector::DistSquared(NewLocation, SmoothMoveTargetLocation) < 10.0f) {
			SetActorLocation(SmoothMoveTargetLocation); // 이동값 정확히 스냅
			SetActorRotation(SmoothMoveTargetRotation); // 회전값 정확히 스냅
			bIsMoving = false;

			if (bIsAlly)
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					TimerHandle,
					this,
					&ATurnBasedUnit::ProcessNextAction,
					0.2f,
					false
				);
			}
			else
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					TimerHandle,
					this,
					&ATurnBasedUnit::EndTurn,
					0.2f,
					false
				);
			}
		}
	}
}
