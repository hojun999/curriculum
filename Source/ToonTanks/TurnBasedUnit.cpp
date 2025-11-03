// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedUnit.h"
#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"
#include "GridManager.h"


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

	// BP에서 UI 클래스가 지정되었느지 확인
	//if (ActionWidgetClass) {
		// 이 폰을 조종하는 플레이어 컨트롤러 가져옴
		//APlayerController* PlayerController = Cast<APlayerController>(GetController());

		// AI가 조종하는 폰이 아닌, 플레이어가 조종하는 폰일 때만 UI 생성
		//if (PlayerController) {
		//	// 위젯 생성 및 변수에 저장
		//	ActionWidgetInstance = CreateWidget<UUserWidget>(PlayerController, ActionWidgetClass);
		//}
	//}

}

void ATurnBasedUnit::OnTurnStarted_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn started."), *GetName());
	bIsMyTurn = true;	// 내 턴이라고 상태만 변경한 후 대기
	bIsExecutingActions = false;

	// 턴 시작 시 행동력 및 액션 큐 초기화
	CurrentActionPoints = MaxActionPoints;
	ActionQueue.Empty();

	// 턴 시작 시 저장해둔 UI 인스턴스를 뷰포트에 추가
	//if (ActionWidgetInstance) {
	//	ActionWidgetInstance->AddToViewport();
	//}

	// TODO 1 : 행동을 위한 UI활성화나 상태 변경 로직 추가
	// 행동이 끝나면 반드시 TurnManager에게 알려줘야됨
	// ex : Atack()이 끝나면 TurnManager->OnUnitActionFinished(); 호출
	// 지금은 즉시 행동을 마쳤다고 가정

	// TODO 2 : 행동을 위한 grid UI 활성화
}

void ATurnBasedUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 입력 바인딩 예시 (실제 게임에서는 UI 버튼 등에서 호출될 수 있음)
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ATurnBasedUnit::MoveUp);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ATurnBasedUnit::MoveDown);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATurnBasedUnit::MoveLeft);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ATurnBasedUnit::MoveRight);
}

// 입력 바인딩용 래퍼 함수들
void ATurnBasedUnit::AddMoveUpAction() { ActionQueue.Enqueue(EUnitAction::MoveUp); }
void ATurnBasedUnit::AddMoveDownAction() { ActionQueue.Enqueue(EUnitAction::MoveDown); }
void ATurnBasedUnit::AddMoveLeftAction() { ActionQueue.Enqueue(EUnitAction::MoveLeft); }
void ATurnBasedUnit::AddMoveRightAction() { ActionQueue.Enqueue(EUnitAction::MoveRight); }


void ATurnBasedUnit::OnTurnEnded_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn ended."), *GetName());
	bIsMyTurn = false;	// 내 턴이 끝났음을 표시

	// 턴 종료 시 뷰포트에서 UI 제거
	//if (ActionWidgetInstance && ActionWidgetInstance->IsInViewport()) {
	//	ActionWidgetInstance->RemoveFromParent();
	//}
}

void ATurnBasedUnit::Initialize(FIntPoint StartCoordinate) {
	CurrentGridCoordinate = StartCoordinate;
}

void ATurnBasedUnit::MoveUp()
{
	// 위쪽 타겟 좌표 계산(Y 1 증가)
	AttemptMove(CurrentGridCoordinate + FIntPoint(0, 1));
}

void ATurnBasedUnit::MoveDown()
{
	AttemptMove(CurrentGridCoordinate + FIntPoint(0, -1));
}

void ATurnBasedUnit::MoveLeft()
{
	AttemptMove(CurrentGridCoordinate + FIntPoint(-1, 0));
}

void ATurnBasedUnit::MoveRight()
{
	AttemptMove(CurrentGridCoordinate + FIntPoint(1, 0));
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
		TargetWorldLocation.Z += 70.0f;
		SetActorLocation(TargetWorldLocation);

		// 나의 현재 그리드 좌표 갱신
		CurrentGridCoordinate = TargetCoordinate;

		return true;
	}
	
	return false;
}

void ATurnBasedUnit::PerformAction() {
	// 내 턴이 아니라면 행동하지 않음
	if (!bIsMyTurn) {
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s performs an action and ends its turn."), *GetName());

	// TODO: 여기에 실제 이동, 공격 등의 로직을 구현합니다.
	// 애니메이션이나 이동이 모두 끝난 뒤에 아래 함수를 호출해야 합니다.

	if (TurnManager)
	{
		TurnManager->OnUnitActionFinished();
	}
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

