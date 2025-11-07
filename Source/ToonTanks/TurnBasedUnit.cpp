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
	ATurnManager* FoundManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATurnManager::StaticClass()));

	if (FoundManager) {
		FoundManager->RegisterUnit(this);
	}

	GridManagerRef = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
}

ATurnManager* ATurnBasedUnit::GetTurnManager()
{
	// 1. 변수가 이미 유효한지(찾아뒀는지) 확인
	if (TurnManager)
	{
		return TurnManager;
	}

	// 2. 변수가 NULL이라면, 지금 월드에서 다시 찾기
	TurnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATurnManager::StaticClass()));

	// 3. 찾았는지 확인
	if (TurnManager)
	{
		return TurnManager;
	}

	// 4. 그래도 못 찾았다면 에러 로그 출력
	UE_LOG(LogTemp, Error, TEXT("GetTurnManager() FAILED: TurnManager Actor could not be found in the world."));
	return nullptr;
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
	
	// 1. 현재 폰의 회전값과 '앞' 방향을 가져옴
	const FRotator CurrentRotation = GetActorRotation();
	const FVector ForwardVector = CurrentRotation.Vector();

	// 2. '앞' 방향을 가장 가까운 그리드 축 방향으로 변환
	const FIntPoint ForwardDirection = FIntPoint(FMath::RoundToInt(ForwardVector.X),
		FMath::RoundToInt(ForwardVector.Y)
	);

	// 3. 현재 방향 기준으로 상하좌우 방향 미리 계산
	const FIntPoint BackwardDirection = ForwardDirection * -1;
	const FIntPoint LeftDirection = FIntPoint(ForwardDirection.Y, -ForwardDirection.X);
	const FIntPoint RightDirection = FIntPoint(-ForwardDirection.Y, ForwardDirection.X);

	// 이동할 목표 좌표
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

	// 4. 어떤 행동이냐에 따라 '목표 회전값'과 '목표 좌표' 설정
	switch (NextAction) {
	case EUnitAction::MoveUp:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Up (Forward) ---"));
		// "UP은 회전 없이 전진"
		SmoothMoveTargetRotation = CurrentRotation; // 현재 각도 유지
		TargetCoordinate += ForwardDirection;
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::MoveDown:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Down (Backward) ---"));
		// "DOWN은 180도 회전"
		SmoothMoveTargetRotation = FRotator(0.0f, CurrentRotation.Yaw + 180.0f, 0.0f);
		TargetCoordinate += BackwardDirection;
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::MoveLeft:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Left ---"));
		// "LEFT는 좌측 90도 회전"
		SmoothMoveTargetRotation = FRotator(0.0f, CurrentRotation.Yaw - 90.0f, 0.0f);
		TargetCoordinate += LeftDirection;
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::MoveRight:
		UE_LOG(LogTemp, Warning, TEXT("--- Move Right ---"));
		// "RIGHT는 우측 90도 회전"
		SmoothMoveTargetRotation = FRotator(0.0f, CurrentRotation.Yaw + 90.0f, 0.0f);
		TargetCoordinate += RightDirection;
		bActionSuccess = AttemptMove(TargetCoordinate);
		break;
	case EUnitAction::Attack:
		UE_LOG(LogTemp, Warning, TEXT("--- Attack Action ---"));
		HandleAttackAction();
		return;
	}

	if (!bActionSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Move action failed (e.g., blocked). Processing next action in queue."));

		// Tick에서 호출하는 것과 동일하게 타이머로 약간의 딜레이 후 다음 행동 처리
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&ATurnBasedUnit::ProcessNextAction,
			0.2f, // Tick과 동일한 딜레이
			false
		);
	}

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
	if (GetTurnManager()) {
		GetTurnManager()->OnUnitActionFinished();
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

		// 이동 및 회전 시작
		SmoothMoveTargetLocation = TargetWorldLocation;
		bIsMoving = true; // 이동 시작

		// 타일 점유 상태 업데이트
		GridManagerRef->UpdateTileOccupancy(CurrentGridCoordinate, TargetCoordinate, this);

		// 나의 현재 그리드 좌표 갱신
		CurrentGridCoordinate = TargetCoordinate;

		return true;
	}
	
	return false;
}

void ATurnBasedUnit::HandleAttackAction()
{
	// 기본 TurnBasedUnit은 공격 시 아무것도 하지 않음.
	// 큐가 멈추지 않도록 다음 큐를 즉시 호출하거나 턴을 종료해야 함.
	// 하지만 BasePawn이 항상 override할 것이므로 비워두거나,
	// 안전을 위해 ProcessNextAction을 호출
	ProcessNextAction();
}

void ATurnBasedUnit::ExecuteEnemyAI()
{
	// 즉시 행동하게 하려면 아래 타이머 삭제
	FTimerHandle AiActionTimer;
	GetWorld()->GetTimerManager().SetTimer(AiActionTimer, [this]() {

		// 1. AI도 플레이어처럼 현재 방향을 기준으로 로직을 수행
		const FRotator CurrentRotation = GetActorRotation();
		const FVector ForwardVector = CurrentRotation.Vector();
		const FIntPoint ForwardDirection = FIntPoint(FMath::RoundToInt(ForwardVector.X), FMath::RoundToInt(ForwardVector.Y));
		const FIntPoint BackwardDirection = ForwardDirection * -1;
		const FIntPoint LeftDirection = FIntPoint(ForwardDirection.Y, -ForwardDirection.X);
		const FIntPoint RightDirection = FIntPoint(-ForwardDirection.Y, ForwardDirection.X);

		FIntPoint TargetCoordinate; // AI가 이동할 목표 좌표

		int32 RandomDirection = FMath::RandRange(0, 3);
		bool bMoveSuccess = false;

		// 2. 랜덤 방향에 따라 '목표 회전값'과 '목표 좌표'를 설정
		switch (RandomDirection)
		{
		case 0: // MoveUp (Forward)
			SmoothMoveTargetRotation = CurrentRotation;
			TargetCoordinate = CurrentGridCoordinate + ForwardDirection;
			break;
		case 1: // MoveDown (Backward)
			SmoothMoveTargetRotation = FRotator(0.0f, CurrentRotation.Yaw + 180.0f, 0.0f);
			TargetCoordinate = CurrentGridCoordinate + BackwardDirection;
			break;
		case 2: // MoveLeft
			SmoothMoveTargetRotation = FRotator(0.0f, CurrentRotation.Yaw - 90.0f, 0.0f);
			TargetCoordinate = CurrentGridCoordinate + LeftDirection;
			break;
		case 3: // MoveRight
			SmoothMoveTargetRotation = FRotator(0.0f, CurrentRotation.Yaw + 90.0f, 0.0f);
			TargetCoordinate = CurrentGridCoordinate + RightDirection;
			break;
		}

		// 3. 설정된 목표 좌표로 이동 시도
		bMoveSuccess = AttemptMove(TargetCoordinate);

		if (bMoveSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("AI Unit %s moved."), *GetName());
			// [참고] 이동에 성공하면 Tick 함수가 bIsAlly=false를 감지하고
			// 자동으로 EndTurn을 호출해줍니다.
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("AI Unit %s failed to move (blocked)."), *GetName());
			EndTurn(); // 이동에 실패하면(길이 막힘) 즉시 턴 종료
		}

		}, 0.2f, false);
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
