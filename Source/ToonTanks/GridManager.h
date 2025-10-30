// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"

#include "GridManager.generated.h"

UCLASS()
class TOONTANKS_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridManager();

	// �����Ϳ��� �Ӽ��� ����� �� ȣ��Ǵ� �Լ�
	virtual void OnConstruction(const FTransform& Transform) override;

	// �׸��� ��ǥ�� �ش��ϴ� Ÿ���� ���� �߾� ��ġ ��ȯ
	// TileCoordinate ã���� �ϴ� Ÿ���� �۸��� ��ǥ (X, Y)
	// WorldLocation ã�� ���� ��ġ�� ���� ����
	// ��ġ�� ���������� ã���� true, �ش� ��ǥ�� Ÿ���� ������ false ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool GetTileWorldLocation(FIntPoint TileCoordinate, FVector& WorldLocation) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> SpawnedTiles;	// ������ Ÿ�ϵ��� ������ �迭

	// �׸��� ��ǥ(FIntPoint)�� Ű��, ���� ��ġ(FVector)�� ������ �����ϴ� TMap
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid Data")
	TMap<FIntPoint, FVector> TileLocations;

	// �׸��� ���� �Լ�
	void GenerateGrid();
	void ClearGrid();	// ������ �׸��带 �����ϴ� �Լ�

	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridWidth = 10; // �׸��� ���� ũ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridHeight = 10; // �׸��� ���� ũ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float TileSpacing = 100.0f; // Ÿ�� ������ ����


private:

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
