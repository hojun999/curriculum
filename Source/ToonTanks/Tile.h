// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "TurnBasedUnit.h"

#include "Tile.generated.h"

class ATurnBasedUnit;

UCLASS()
class TOONTANKS_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	UStaticMeshComponent* TileMesh;

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetTileColor(FLinearColor NewColor);

	// 해당 타일에 있는 유닛
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile State")
	ATurnBasedUnit* OccupyingUnit = nullptr;

	// 해당 타일이 점유되었는지 확인하는 함수
	UFUNCTION(BlueprintPure, Category = "Tile State")
	bool IsOccupied() const {
		return OccupyingUnit != nullptr;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
