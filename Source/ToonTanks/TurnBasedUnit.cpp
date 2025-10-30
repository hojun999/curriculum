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
}

void ATurnBasedUnit::OnTurnStarted() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn started."), *GetName());
	bIsMyTurn = true;	// 내 턴이라고 상태만 변경한 후 대기

	// TODO : 행동을 위한 UI활성화나 상태 변경 로직 추가
	// 행동이 끝나면 반드시 TurnManager에게 알려줘야됨
	// ex : Atack()이 끝나면 TurnManager->OnUnitActionFinished(); 호출

	// 지금은 즉시 행동을 마쳤다고 가정
	
}

void ATurnBasedUnit::OnTurnEnded() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn ended."), *GetName());
	bIsMyTurn = false;	// 내 턴이 끝났음을 표시
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

void ATurnBasedUnit::AttemptMove(FIntPoint TargetCoordinate)
{
	// 내 턴일 때만 이동 가능
	if (!bIsMyTurn)
		return;

	// GridManager가 월드에 존재하는지 확인
	AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (!GridManager)
		return;

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

		// 턴 종료
		// (이동 후 추가 행동이 가능한 경우 해당 코드 수정(다른 곳으로 옮겨야됨) 필요)
		PerformAction(); // 턴 종료 함수 호출
	}
	else {
		// 이동 불가능한 위치일 경우
		UE_LOG(LogTemp, Warning, TEXT("Cannot move to %s. It's an invalid location."), *TargetCoordinate.ToString());
	}

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
void ATurnBasedUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

