// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoreDatas.h"	// ������ ����ü ���
#include "TurnBasedGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATurnBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	// ������ ������ ���� �迭 ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage Setup")
	TArray<FPawnSpawnInfo> PawnSpawnInfos;

private:
	// �������� ���� �� ������ �����ϴ� �Լ�
	void SpawnPawnsForStage();
};
