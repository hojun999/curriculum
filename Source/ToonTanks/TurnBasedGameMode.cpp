// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedGameMode.h"
#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "TurnBasedUnit.h"
#include "TurnManager.h"

void ATurnBasedGameMode::BeginPlay()
{
	Super::BeginPlay();

	// �������� ����
	SpawnPawnsForStage();
}

void ATurnBasedGameMode::SpawnPawnsForStage()
{
	// ���忡 ��ġ�� GridManager ã��
	AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));

	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("SpawnPawnsForStage: GridManager not found in the world!"));
		return;
	}

	// ������ ���� ���� �迭�� ��ȸ�ϸ� �� ����
	for (int32 i = 0; i < PawnSpawnInfos.Num(); i++)
	{
		const FPawnSpawnInfo& SpawnInfo = PawnSpawnInfos[i];

		if (!SpawnInfo.PawnClass) {
			continue;	// ������ �� Ŭ������ �������� �ʾ����� �ǳʶٱ�
		}

		FVector SpawnLocation;
		// GridManager���� �׸��� ��ǥ�� �ش�Ǵ� ���� ��ġ �޾ƿ���
		if (GridManager->GetTileWorldLocation(SpawnInfo.SpawnCoordinate, SpawnLocation)) {
			// ��ġ ã�⿡ ���������� �ش� ��ġ�� �� ����
			UE_LOG(LogTemp, Warning, TEXT("Found spawn location for %s"), *SpawnInfo.SpawnCoordinate.ToString());

			// �浹 ������ ���� ��ġ�� ����
			SpawnLocation.Z += 35.0f;

			// ���� �Ķ���� ���� ����
			FActorSpawnParameters SpawnParams;

			// �ݸ��� �浹 ó�� ��� ����
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ATurnBasedUnit* SpawnedUnit = GetWorld()->SpawnActor<ATurnBasedUnit>(SpawnInfo.PawnClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

			if (SpawnedUnit)
			{
				// ������ ���ֿ��� ���� �׸��� ��ǥ ����
				SpawnedUnit->Initialize(SpawnInfo.SpawnCoordinate);

				// ù ��°�� ������ ���� �÷��̾� ������ �����ϰ� ����
				// TODO : ���Ŀ� �ٸ� ���� �÷��̾� ������ �����ϴ� ���� �߰�
				if (i == 0) {
					APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
					if (PlayerController) {
						PlayerController->Possess(SpawnedUnit);
					}
				}
			}
			else {
				// AlwaysSpawn���ε� �����ϸ� �ٸ� ������ �ִ� ������ Ȯ�� �ʿ�
				UE_LOG(LogTemp, Error, TEXT("Spawn FAILED for PawnClass at location %s"), *SpawnLocation.ToString());
			}

		}
		else {	// ��ġ ã�⿡ �������� ��� �α� ���
			UE_LOG(LogTemp, Warning, TEXT("Could not find world location for grid coordinate: %s"), *SpawnInfo.SpawnCoordinate.ToString());
		}
	}

	ATurnManager* TurnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATurnManager::StaticClass()));
	if (TurnManager) {
		TurnManager->StartCombat();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("SpawnPawnsForStage: TurnManager not found!"));
	}
}
