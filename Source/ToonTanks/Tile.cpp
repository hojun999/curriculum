// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = TileMesh;

	// ������ ����� �⺻ Plane �޽� ���� ã��
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneAsset(TEXT("/Engine/BasicShapes/Plane"));

	// ������ ���������� ã������, TileMesh�� ����ƽ �޽÷� �Ҵ�
	if (PlaneAsset.Succeeded()) {
		TileMesh->SetStaticMesh(PlaneAsset.Object);
		TileMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}

void ATile::SetTileColor(FLinearColor NewColor)
{
	// �����ص� ���� ��Ƽ������ ���� �Ķ���� ����
	if (DynamicMaterialInstance) {
		// ��Ƽ���� "BaseColor"��� �̸��� Vector �Ķ���Ͱ� �־�� ��
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

