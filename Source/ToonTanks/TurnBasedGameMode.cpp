// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedGameMode.h"
#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "TurnBasedUnit.h"
#include "TurnManager.h"

void ATurnBasedGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 스테이지 시작
	SpawnPawnsForStage();
}

void ATurnBasedGameMode::SpawnPawnsForStage()
{
	// 월드에 배치된 GridManager 찾기
	AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));

	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("SpawnPawnsForStage: GridManager not found in the world!"));
		return;
	}

	// 설정된 스폰 정보 배열을 순회하며 폰 스폰
	for (int32 i = 0; i < PawnSpawnInfos.Num(); i++)
	{
		const FPawnSpawnInfo& SpawnInfo = PawnSpawnInfos[i];

		if (!SpawnInfo.PawnClass) {
			continue;	// 스폰할 폰 클래스가 지정되지 않았으면 건너뛰기
		}

		FVector SpawnLocation;
		// GridManager에게 그리드 좌표에 해당되는 월드 위치 받아오기
		if (GridManager->GetTileWorldLocation(SpawnInfo.SpawnCoordinate, SpawnLocation)) {
			// 위치 찾기에 성공했으면 해당 위치에 폰 스폰
			UE_LOG(LogTemp, Warning, TEXT("Found spawn location for %s"), *SpawnInfo.SpawnCoordinate.ToString());

			// 충돌 방지를 위한 위치값 변경
			SpawnLocation.Z += 35.0f;

			// 스폰 파라미터 변수 생성
			FActorSpawnParameters SpawnParams;

			// 콜리젼 충돌 처리 방식 설정
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ATurnBasedUnit* SpawnedUnit = GetWorld()->SpawnActor<ATurnBasedUnit>(SpawnInfo.PawnClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

			if (SpawnedUnit)
			{
				// 스폰된 유닛에게 시작 그리드 좌표 전달
				SpawnedUnit->Initialize(SpawnInfo.SpawnCoordinate);

				// 첫 번째로 스폰된 폰을 플레이어 폰으로 가정하고 빙의
				// TODO : 이후에 다른 폰을 플레이어 폰으로 지정하는 로직 추가
				if (i == 0) {
					APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
					if (PlayerController) {
						PlayerController->Possess(SpawnedUnit);
					}
				}
			}
			else {
				// AlwaysSpawn으로도 실패하면 다른 문제가 있는 것으로 확인 필요
				UE_LOG(LogTemp, Error, TEXT("Spawn FAILED for PawnClass at location %s"), *SpawnLocation.ToString());
			}

		}
		else {	// 위치 찾기에 실패했을 경우 로그 출력
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
