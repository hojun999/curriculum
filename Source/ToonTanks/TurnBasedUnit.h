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

	///// ---------- ����

	// �� ������ ���� ���� ������ �ִ��� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	bool bIsMyTurn = false;

	// ������ �Ʊ�/�� ���¸� �����Ϳ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	bool bIsAlly = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FIntPoint CurrentGridCoordinate;

	////// ---------- �Լ�

	// ������ �̵��̳� ���� ���� �ൿ�� �����ϴ� �Լ�
	void PerformAction();

	// GameMode�� ������ ������ ���� ȣ���� �ʱ�ȭ �Լ�
	void Initialize(FIntPoint StartCoordinate);

	// ���� ���۵Ǿ��� �� TurnManager�� ȣ���� �Լ�
	virtual void OnTurnStarted();

	// ���� ����Ǿ��� �� ȣ��� �Լ�
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
	// ���� �̵��� �����ϴ� ���� �Լ�
	void AttemptMove(FIntPoint TargetCoordinate);

protected:
	// ��� ������ TurnManager�� ���� ������ �� �ֵ��� ���� ����
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
