// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "Tile.h"

// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AGridManager::GetTileWorldLocation(FIntPoint TileCoordinate, FVector& WorldLocation) const
{
	// TMap에서 해당 좌표에 대한 월드 위치 값 찾기
	const FVector* FoundLocation = TileLocations.Find(TileCoordinate);

	if (FoundLocation) {
		WorldLocation = *FoundLocation;
		return true;
	}
	return false;
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	//OnConstruction에서 생성된 에디터용 타일 정보를 비우고 다시 생성
	ClearGrid();
	GenerateGrid();
}

void AGridManager::ClearGrid()
{
	// 배열에 저장된 모든 타일 액터 순회 및 파괴
	for (AActor* Tile : SpawnedTiles)
	{
		if (Tile) {
			Tile->Destroy();
		}
	}
	// 배열 비우기
	SpawnedTiles.Empty();

	//TMap 비우기
	TileLocations.Empty();
}

void AGridManager::GenerateGrid()
{
	if (!TileClass) {
		return;
	}

	// 그리드 생성
	for (int32 y = 0; y < GridHeight; y++)
	{
		for (int32 x = 0; x < GridWidth; x++)
		{
			// 그리드 좌표 생성
			const FIntPoint GridCoordinate = FIntPoint(x, y);

			// 타일 스폰 위치 계산
			const FVector SpawnLocation = FVector(x * TileSpacing, y * TileSpacing, 0.0f) + GetActorLocation();

			// TMap에 위치 정보 저장
			TileLocations.Add(GridCoordinate, SpawnLocation);

			// 월드에 Tile 스폰 및 SpawnedTiles 배열에 추가
			// FActorSpawnParameters를 사용하여 GridManager가 스폰한 액터임을 명시
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			ATile* SpawnedTile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator);

			if (SpawnedTile) {
				SpawnedTiles.Add(SpawnedTile);
			}
		}
	}
}

void AGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 기존에 생성된 타일들 전부 삭제
	ClearGrid();

	// 새로운 그리드 생성
	GenerateGrid();
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

