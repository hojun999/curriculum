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
	
	// ������ ���۵Ǹ� ������ TurnManager�� ã�� ����
	TurnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATurnManager::StaticClass()));
}

void ATurnBasedUnit::OnTurnStarted() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn started."), *GetName());
	bIsMyTurn = true;	// �� ���̶�� ���¸� ������ �� ���

	// TODO : �ൿ�� ���� UIȰ��ȭ�� ���� ���� ���� �߰�
	// �ൿ�� ������ �ݵ�� TurnManager���� �˷���ߵ�
	// ex : Atack()�� ������ TurnManager->OnUnitActionFinished(); ȣ��

	// ������ ��� �ൿ�� ���ƴٰ� ����
	
}

void ATurnBasedUnit::OnTurnEnded() {
	UE_LOG(LogTemp, Log, TEXT("%s's turn ended."), *GetName());
	bIsMyTurn = false;	// �� ���� �������� ǥ��
}

void ATurnBasedUnit::Initialize(FIntPoint StartCoordinate) {
	CurrentGridCoordinate = StartCoordinate;
}

void ATurnBasedUnit::MoveUp()
{
	// ���� Ÿ�� ��ǥ ���(Y 1 ����)
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
	// �� ���� ���� �̵� ����
	if (!bIsMyTurn)
		return;

	// GridManager�� ���忡 �����ϴ��� Ȯ��
	AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (!GridManager)
		return;

	FVector TargetWorldLocation;
	// GridManager���� ��ǥ ��ǥ �̵� ���� ���� Ȯ��
	if (GridManager->GetTileWorldLocation(TargetCoordinate, TargetWorldLocation)) {
		// �̵� ������ ��� �ش� ��ġ�� ���� �̵�
		UE_LOG(LogTemp, Log, TEXT("%s moves from %s to %s"), *GetName(), *CurrentGridCoordinate.ToString(), *TargetCoordinate.ToString());


		// ��ġ �������� �ε巴�� �̵�
		TargetWorldLocation.Z += 70.0f;
		SetActorLocation(TargetWorldLocation);

		// ���� ���� �׸��� ��ǥ ����
		CurrentGridCoordinate = TargetCoordinate;

		// �� ����
		// (�̵� �� �߰� �ൿ�� ������ ��� �ش� �ڵ� ����(�ٸ� ������ �Űܾߵ�) �ʿ�)
		PerformAction(); // �� ���� �Լ� ȣ��
	}
	else {
		// �̵� �Ұ����� ��ġ�� ���
		UE_LOG(LogTemp, Warning, TEXT("Cannot move to %s. It's an invalid location."), *TargetCoordinate.ToString());
	}

}

void ATurnBasedUnit::PerformAction() {
	// �� ���� �ƴ϶�� �ൿ���� ����
	if (!bIsMyTurn) {
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s performs an action and ends its turn."), *GetName());

	// TODO: ���⿡ ���� �̵�, ���� ���� ������ �����մϴ�.
	// �ִϸ��̼��̳� �̵��� ��� ���� �ڿ� �Ʒ� �Լ��� ȣ���ؾ� �մϴ�.

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

