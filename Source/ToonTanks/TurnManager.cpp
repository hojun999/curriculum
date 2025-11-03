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
//	// 월드에 있는 모든 아군과 적군 유닛을 찾아 배열에 등록
//	// GetAllActorsOfClass >> 스폰될 때 각 유닛이 스스로 등록되는 방식으로 변경 필요
//	TArray<AActor*> FoundUnits;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurnBasedUnit::StaticClass(), FoundUnits);
//
//	for (AActor* UnitActor : FoundUnits)
//	{
//		ATurnBasedUnit* Unit = Cast<ATurnBasedUnit>(UnitActor);
//		if (Unit) {
//			if (Unit->bIsAlly) {
//				AllyUnits.Add(Unit);
//			}
//			else {
//				EnemyUnits.Add(Unit);
//			}
//		}
//	}
//
//	// 아군 턴부터 시작
	StartAllyTurn();
}

void ATurnManager::RegisterUnit(ATurnBasedUnit* Unit)
{
	if (!Unit) {
		return;
	}

	if (Unit->bIsAlly) {
		AllyUnits.Add(Unit);
	}
	else {
		EnemyUnits.Add(Unit);
	}
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
	// TODO : 적군 AI 로직 실행
	// 우선 모든 적이 행동을 마쳤다고 가정하고 턴 넘기기
	OnUnitActionFinished();
}

void ATurnManager::NextUnitAction()
{
	TArray<ATurnBasedUnit*>& CurrentTeam = (CurrentTurn == ETurnState::E_AllyTurn) ? AllyUnits : EnemyUnits;

	if (CurrentTeam.IsValidIndex(CurrentUnitIndex)) {
		// 현재 순서의 유닛에게 턴이 시작되었다고 알림
		CurrentTeam[CurrentUnitIndex]->OnTurnStarted();
	}
	else {	// 현재 팀의 모든 유닛이 행동을 마쳤다면
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
	// 현재 유닛의 턴이 끝났음을 알림
	TArray<ATurnBasedUnit*>& CurrentTeam = (CurrentTurn == ETurnState::E_AllyTurn) ? AllyUnits : EnemyUnits;
	if (CurrentTeam.IsValidIndex(CurrentUnitIndex)) {
		CurrentTeam[CurrentUnitIndex]->OnTurnEnded();
	}

	// 다음 유닛으로 인덱스 넘김
	CurrentUnitIndex++;

	// 다음 유닛의 행동 시작
	NextUnitAction();
}




// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

