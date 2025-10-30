// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CoreDatas.generated.h"

// ������ ��ġ�� �����ϴ� ����ü
USTRUCT(BlueprintType)
struct FPawnSpawnInfo {
	GENERATED_BODY()

	// ������ ���� BP Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APawn> PawnClass;

	// ������ Ÿ���� �׸��� ��ǥ
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntPoint SpawnCoordinate;
};

UENUM(BlueprintType)
enum class ETurnState : uint8 {
	E_AllyTurn UMETA(DisplayName = "Ally Turn"),
	E_EnemyTurn UMETA(DisplayName = "Enemy Turn"),
	E_Processing UMETA(DisplayName = "Processing")	// �� ��ȯ �� ó�� ���� ��Ÿ��
};

class TOONTANKS_API CoreDatas
{
public:
	CoreDatas();
	~CoreDatas();
};
