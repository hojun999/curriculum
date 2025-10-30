// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "TurnBasedUnit.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATurnManager::ATurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();
	
	//StartCombat();
}

void ATurnManager::StartCombat()
{
	// ���忡 �ִ� ��� �Ʊ��� ���� ������ ã�� �迭�� ���
	// GetAllActorsOfClass >> ������ �� �� ������ ������ ��ϵǴ� ������� ���� �ʿ�
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurnBasedUnit::StaticClass(), FoundUnits);

	for (AActor* UnitActor : FoundUnits)
	{
		ATurnBasedUnit* Unit = Cast<ATurnBasedUnit>(UnitActor);
		if (Unit) {
			if (Unit->bIsAlly) {
				AllyUnits.Add(Unit);
			}
			else {
				EnemyUnits.Add(Unit);
			}
		}
	}

	// �Ʊ� �Ϻ��� ����
	StartAllyTurn();
}

void ATurnManager::StartAllyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("--- Ally Turn Started ---"));

	CurrentTurn = ETurnState::E_AllyTurn;
	CurrentUnitIndex = 0;
	NextUnitAction();
}

void ATurnManager::StartEnemyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("--- Enemy Turn Started ---"));

	CurrentTurn = ETurnState::E_EnemyTurn;
	CurrentUnitIndex = 0;
	// TODO : ���� AI ���� ����
	// �켱 ��� ���� �ൿ�� ���ƴٰ� �����ϰ� �� �ѱ��
	OnUnitActionFinished();
}

void ATurnManager::NextUnitAction()
{
	TArray<ATurnBasedUnit*>& CurrentTeam = (CurrentTurn == ETurnState::E_AllyTurn) ? AllyUnits : EnemyUnits;

	if (CurrentTeam.IsValidIndex(CurrentUnitIndex)) {
		// ���� ������ ���ֿ��� ���� ���۵Ǿ��ٰ� �˸�
		CurrentTeam[CurrentUnitIndex]->OnTurnStarted();
	}
	else {	// ���� ���� ��� ������ �ൿ�� ���ƴٸ�
		if (CurrentTurn == ETurnState::E_AllyTurn) {
			StartEnemyTurn();
		}
		else {
			StartAllyTurn();
		}
	}
}

void ATurnManager::OnUnitActionFinished()
{
	// ���� ������ ���� �������� �˸�
	TArray<ATurnBasedUnit*>& CurrentTeam = (CurrentTurn == ETurnState::E_AllyTurn) ? AllyUnits : EnemyUnits;
	if (CurrentTeam.IsValidIndex(CurrentUnitIndex)) {
		CurrentTeam[CurrentUnitIndex]->OnTurnEnded();
	}

	// ���� �������� �ε��� �ѱ�
	CurrentUnitIndex++;

	// ���� ������ �ൿ ����
	NextUnitAction();
}




// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

