// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnBasedUnit.generated.h"

class ATurnManager;

UCLASS()
class TOONTANKS_API ATurnBasedUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATurnBasedUnit();

	///// ---------- 변수

	// 이 유닛이 현재 턴을 가지고 있는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	bool bIsMyTurn = false;

	// 유닛의 아군/적 상태를 에디터에서 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	bool bIsAlly = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FIntPoint CurrentGridCoordinate;

	////// ---------- 함수

	// 유닛이 이동이나 공격 같은 행동을 수행하는 함수
	void PerformAction();

	// GameMode가 유닛을 스폰한 직수 호출할 초기화 함수
	void Initialize(FIntPoint StartCoordinate);

	// 턴이 시작되었을 때 TurnManager가 호출할 함수
	virtual void OnTurnStarted();

	// 턴이 종료되었을 때 호출될 함수
	virtual void OnTurnEnded();

	UFUNCTION(BlueprintCallable, Category = "Movemnet")
	void MoveUp();

	UFUNCTION(BlueprintCallable, Category = "Movemnet")
	void MoveDown();

	UFUNCTION(BlueprintCallable, Category = "Movemnet")
	void MoveLeft();

	UFUNCTION(BlueprintCallable, Category = "Movemnet")
	void MoveRight();

private:
	// 실제 이동을 수행하는 내부 함수
	void AttemptMove(FIntPoint TargetCoordinate);

protected:
	// 모든 유닛이 TurnManager에 쉽게 접근할 수 있도록 참조 저장
	UPROPERTY()
	ATurnManager* TurnManager;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
