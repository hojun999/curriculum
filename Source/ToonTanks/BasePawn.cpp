// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"

ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
	bUseControllerRotationYaw = true;
}


void ABasePawn::HandleDestruction()
{
	// TODO : Visual/sound effects
	if (DeathParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());
	}

	if (DeathSound) {
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathCameraShakeClass) {
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}
}

void ABasePawn::OnTurnStarted_Implementation()
{
	Super::OnTurnStarted_Implementation();
}

void ABasePawn::OnTurnEnded_Implementation()
{
	Super::OnTurnEnded_Implementation();
}

void ABasePawn::HandleAttackAction()
{
	ATurnBasedUnit* TargetEnemy = FindClosestEnemy();
	FVector TargetLocation;
	bShotWillDealDamage = false;

	if (TargetEnemy) {
		float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

		if (Distance <= MaxAttackRange) {
			// 사거리 내
			UE_LOG(LogTemp, Log, TEXT("Enemy in range. Attacking %s"), *TargetEnemy->GetName());
			TargetLocation = TargetEnemy->GetActorLocation();
			bShotWillDealDamage = true; // 데미지를 입히는 공격
		}
		else {
			// 사거리 밖
			UE_LOG(LogTemp, Log, TEXT("Enemy out of range. Firing at max range."));
			FVector Direction = (TargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			TargetLocation = GetActorLocation() + Direction * MaxAttackRange;
			bShotWillDealDamage = false;
		}
	}
	else {
		// 적 없음 (최대 사거리에 발사)
		UE_LOG(LogTemp, Log, TEXT("No enemies. Firing forward."));
		TargetLocation = GetActorLocation() + GetActorForwardVector() * MaxAttackRange;
		bShotWillDealDamage = false;
	}

	// 조준 및 발사 시작
	AimAndFire(TargetLocation, bShotWillDealDamage);
}

ATurnBasedUnit* ABasePawn::FindClosestEnemy()
{
	if (!TurnManager)
		return nullptr;

	// TurnManager가 가지고 있는 적 목록을 가져옴(bIsAlly 값에 따라 반대)
	TArray<ATurnBasedUnit*>& EnemyList = (bIsAlly) ? TurnManager->EnemyUnits : TurnManager->AllyUnits;
	if (EnemyList.Num() == 0)
		return nullptr;

	ATurnBasedUnit* ClosestEnemy = nullptr;
	float MinDistSq = MAX_FLT; // 최대 float 값으로 초기화

	for (ATurnBasedUnit* Enemy : EnemyList) {
		// 적 생존 확인
		ABasePawn* EnemyPawn = Cast<ABasePawn>(Enemy);
		if (!EnemyPawn || EnemyPawn->Health <= 0.0f) continue;

		float DistSq = FVector::DistSquared(GetActorLocation(), Enemy->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestEnemy = Enemy;
		}
	}
	return ClosestEnemy;
}

//void ABasePawn::RotateTurret(FVector LookAtTarget)
//{
//	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
//	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
//	TurretMesh->SetWorldRotation(FMath::RInterpTo(
//		TurretMesh->GetComponentRotation(),
//		LookAtRotation,
//		UGameplayStatics::GetWorldDeltaSeconds(this),
//		5.f)
//	);
//}

void ABasePawn::Fire() {
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);
}

