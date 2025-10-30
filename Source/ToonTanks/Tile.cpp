// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = TileMesh;

	// 엔진에 내장된 기본 Plane 메시 에셋 찾기
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneAsset(TEXT("/Engine/BasicShapes/Plane"));

	// 에셋을 성공적으로 찾았으면, TileMesh의 스태틱 메시로 할당
	if (PlaneAsset.Succeeded()) {
		TileMesh->SetStaticMesh(PlaneAsset.Object);
		TileMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}

void ATile::SetTileColor(FLinearColor NewColor)
{
	// 저장해둔 동적 머티리얼의 색상 파라미터 변경
	if (DynamicMaterialInstance) {
		// 머티리얼에 "BaseColor"라는 이름의 Vector 파라미터가 있어야 됨
		DynamicMaterialInstance->SetVectorParameterValue(FName("BaseColor"), NewColor);
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	if (TileMesh->GetMaterial(0)) {
		DynamicMaterialInstance = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
	}
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

