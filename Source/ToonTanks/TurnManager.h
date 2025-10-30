// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreDatas.h"
#include "TurnManager.generated.h"

class ATurnBasedUnit;	// BasePawn(또는 Tank/Turret)의 부모 클래스로 사용할 클래스

UCLASS()
class TOONTANKS_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnManager();

	// 전투 시작을 알리는 함수
	UFUNCTION(BlueprintCallable)
	void StartCombat();

	// 현재 유닛의 행동 완료 시 호출 함수
	void OnUnitActionFinished();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	ETurnState CurrentTurn;

	// 전투에 참여하는 모든 아군 유닛
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	TArray<ATurnBasedUnit*> AllyUnits;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	TArray<ATurnBasedUnit*> EnemyUnits;

private:
	void StartAllyTurn();
	void StartEnemyTurn();
	void NextUnitAction();

	int32 CurrentUnitIndex;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
