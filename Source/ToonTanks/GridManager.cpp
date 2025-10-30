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
	// TMap���� �ش� ��ǥ�� ���� ���� ��ġ �� ã��
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

	//OnConstruction���� ������ �����Ϳ� Ÿ�� ������ ���� �ٽ� ����
	ClearGrid();
	GenerateGrid();
}

void AGridManager::ClearGrid()
{
	// �迭�� ����� ��� Ÿ�� ���� ��ȸ �� �ı�
	for (AActor* Tile : SpawnedTiles)
	{
		if (Tile) {
			Tile->Destroy();
		}
	}
	// �迭 ����
	SpawnedTiles.Empty();

	//TMap ����
	TileLocations.Empty();
}

void AGridManager::GenerateGrid()
{
	if (!TileClass) {
		return;
	}

	// �׸��� ����
	for (int32 y = 0; y < GridHeight; y++)
	{
		for (int32 x = 0; x < GridWidth; x++)
		{
			// �׸��� ��ǥ ����
			const FIntPoint GridCoordinate = FIntPoint(x, y);

			// Ÿ�� ���� ��ġ ���
			const FVector SpawnLocation = FVector(x * TileSpacing, y * TileSpacing, 0.0f) + GetActorLocation();

			// TMap�� ��ġ ���� ����
			TileLocations.Add(GridCoordinate, SpawnLocation);

			// ���忡 Tile ���� �� SpawnedTiles �迭�� �߰�
			// FActorSpawnParameters�� ����Ͽ� GridManager�� ������ �������� ���
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

	// ������ ������ Ÿ�ϵ� ���� ����
	ClearGrid();

	// ���ο� �׸��� ����
	GenerateGrid();
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

