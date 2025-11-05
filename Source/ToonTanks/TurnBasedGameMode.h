// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoreDatas.h"	// 데이터 구조체 헤더
#include "TurnBasedGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATurnBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	ATurnBasedGameMode();
	virtual void BeginPlay() override;

	// 스폰할 폰들의 정보 배열 노출
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage Setup")
	TArray<FPawnSpawnInfo> PawnSpawnInfos;

private:
	// 스테이지 시작 시 폰들을 스폰하는 함수
	void SpawnPawnsForStage();
};
