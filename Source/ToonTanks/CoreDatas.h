// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CoreDatas.generated.h"

// 스폰할 위치를 관리하는 구조체
USTRUCT(BlueprintType)
struct FPawnSpawnInfo {
	GENERATED_BODY()

	// 스폰할 폰의 BP 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APawn> PawnClass;

	// 스폰될 타일의 그리드 좌표
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntPoint SpawnCoordinate;
};

UENUM(BlueprintType)
enum class ETurnState : uint8 {
	E_AllyTurn UMETA(DisplayName = "Ally Turn"),
	E_EnemyTurn UMETA(DisplayName = "Enemy Turn"),
	E_Processing UMETA(DisplayName = "Processing")	// 턴 전환 등 처리 중을 나타냄
};

class TOONTANKS_API CoreDatas
{
public:
	CoreDatas();
	~CoreDatas();
};
