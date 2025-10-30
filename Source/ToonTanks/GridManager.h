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

	// 에디터에서 속성이 변경될 때 호출되는 함수
	virtual void OnConstruction(const FTransform& Transform) override;

	// 그리드 좌표에 해당하는 타일의 월드 중앙 위치 반환
	// TileCoordinate 찾고자 하는 타일의 글리드 좌표 (X, Y)
	// WorldLocation 찾은 월드 위치를 담을 변수
	// 위치를 성공적으로 찾으면 true, 해당 좌표에 타일이 없으면 false 반환
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool GetTileWorldLocation(FIntPoint TileCoordinate, FVector& WorldLocation) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> SpawnedTiles;	// 생성된 타일들을 저장할 배열

	// 그리드 좌표(FIntPoint)를 키로, 월드 위치(FVector)를 값으로 저장하는 TMap
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid Data")
	TMap<FIntPoint, FVector> TileLocations;

	// 그리드 생성 함수
	void GenerateGrid();
	void ClearGrid();	// 생성된 그리드를 제거하는 함수

	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridWidth = 10; // 그리드 가로 크기

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridHeight = 10; // 그리드 세로 크기

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float TileSpacing = 100.0f; // 타일 사이의 간격


private:

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
