// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreDatas.h"
#include "TurnManager.generated.h"

class ATurnBasedUnit;	// BasePawn(�Ǵ� Tank/Turret)�� �θ� Ŭ������ ����� Ŭ����

UCLASS()
class TOONTANKS_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnManager();

	// ���� ������ �˸��� �Լ�
	UFUNCTION(BlueprintCallable)
	void StartCombat();

	// ���� ������ �ൿ �Ϸ� �� ȣ�� �Լ�
	void OnUnitActionFinished();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	ETurnState CurrentTurn;

	// ������ �����ϴ� ��� �Ʊ� ����
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
