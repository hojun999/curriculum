// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoreDatas.h"	// 데이터 구조체 헤더
#include "TurnBasedGameMode.generated.h"

class UUserWidget;

UCLASS()
class TOONTANKS_API ATurnBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void ActorDied(AActor* DeadActor);

	// 게임 오버 또는 게임 클리어 처리
	void HandleGameOver(bool bPlayerWon);

protected:
	ATurnBasedGameMode();
	virtual void BeginPlay() override;

	// 스폰할 폰들의 정보 배열 노출
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage Setup")
	TArray<FPawnSpawnInfo> PawnSpawnInfos;

	// 월드에 남아있는 적 포탑의 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	int32 EnemyTurretCount = 0;

	// BP에서 설정할 게임 클리어 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameClearWidgetClass;

	// BP에서 설정할 게임 오버 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

private:
	// 스테이지 시작 시 폰들을 스폰하는 함수
	void SpawnPawnsForStage();

};
