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

	// 아군 턴부터 시작
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

	// 아군이 한 명이라도 있는지 확인
	if (AllyUnits.Num() > 0) {
		NextUnitAction(); // 아군이 있으면 첫 번째 유닛 턴 시작
	}
	else {
		StartEnemyTurn(); // 아군이 없으면 적군 턴으로 바로 전환
	}

}

void ATurnManager::StartEnemyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("--- Enemy Turn Started ---"));

	CurrentTurn = ETurnState::E_EnemyTurn;
	CurrentUnitIndex = 0;

	if (EnemyUnits.Num() > 0) {
		// TODO : 적군 AI 로직 실행
		NextUnitAction();
	}
	else {
		StartAllyTurn(); // 적군이 없으면 아군 턴으로 바로 전환
	}

}

void ATurnManager::NextUnitAction()
{
	TArray<ATurnBasedUnit*>& CurrentTeam = (CurrentTurn == ETurnState::E_AllyTurn) ? AllyUnits : EnemyUnits;

	// [핵심 수정 로직]
	// 1. CurrentUnitIndex가 배열 범위(0부터 Num-1까지) 안에 있는지 *먼저* 확인합니다.
	if (CurrentTeam.IsValidIndex(CurrentUnitIndex))
	{
		// 2. 범위 안에 있다면, 해당 포인터가 NULL이 아닌지 확인합니다.
		if (CurrentTeam[CurrentUnitIndex] != nullptr)
		{
			// 둘 다 통과하면 턴 시작
			CurrentTeam[CurrentUnitIndex]->OnTurnStarted();
		}
		else
		{
			// 포인터가 NULL인 드문 경우 (예: 유닛이 중간에 파괴됨)
			// 다음 유닛으로 강제 진행
			CurrentUnitIndex++;
			NextUnitAction();
		}
	}
	else
	{
		// CurrentUnitIndex가 배열 범위를 벗어남 (예: 1번 인덱스인데 배열 크기가 1)
		// 즉, 현재 팀의 모든 유닛이 행동을 마쳤다는 뜻이므로 다음 팀의 턴을 시작합니다.
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
	if (CurrentTeam.IsValidIndex(CurrentUnitIndex) && CurrentTeam[CurrentUnitIndex] != nullptr) {
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

	// 전투가 아직 시작되지 않았고, 아군 또는 적군 유닛이 1명이라도 등록되어있다면 전투 시작
	if(!bIsCombatStarted && (AllyUnits.Num() > 0 || EnemyUnits.Num() > 0)) {
		UE_LOG(LogTemp, Warning, TEXT("--- start combat ---"));

		bIsCombatStarted = true; // 플래그를 올려 중복 실행 방지
		StartCombat();
	}

}

