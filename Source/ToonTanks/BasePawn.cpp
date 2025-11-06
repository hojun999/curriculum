// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "TurnManager.h"
#include "TurnBasedGameMode.h"

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
	bUseControllerRotationYaw = false;
}

void ABasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAiming && !bIsMoving) {
		FVector ToTarget = AimTargetLocation - TurretMesh->GetComponentLocation();
		FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

		FRotator CurrentRotation = TurretMesh->GetComponentRotation();
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,
			LookAtRotation,
			DeltaTime,
			RotationSpeed
		);

		TurretMesh->SetWorldRotation(NewRotation);

		// 2. 조준이 완료되었는지 확인 (1도 이내)
		if (NewRotation.Equals(LookAtRotation, 1.0f))
		{
			bIsAiming = false; // 조준 완료

			// 3. 발사!
			Fire(bShotWillDealDamage);

			// 4. 발사 후, 다음 액션 큐 처리를 위한 타이머 호출
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				this,
				&ABasePawn::ProcessNextAction, // 다음 행동으로 넘어감
				0.5f, // 발사 후 딜레이
				false
			);
		}
	}
}

void ABasePawn::BeginPlay()
{
	Super::BeginPlay();

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

float ABasePawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float DamageToApply = FMath::Min(Health, DamageAmount);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Health remaining: %f"), *GetName(), DamageToApply, Health);

	if (Health <= 0.0f) {
		// 유닛 사망 처리
		HandleDestruction();

		// 턴 매니저에게 사망 등록(배열에서 제거)
		if (GetTurnManager()) {
			GetTurnManager()->UnregisterUnit(this);
		}

		// GameMode에게 내가 죽었음을 알림
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
		if (ATurnBasedGameMode* MyGameMode = Cast<ATurnBasedGameMode>(GameMode))
		{
			MyGameMode->ActorDied(this);
		}

		Destroy();
	}
	return DamageToApply;
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
	// --- 1. TurnManager 확인 ---
	if (!GetTurnManager())
	{
		UE_LOG(LogTemp, Error, TEXT("FindClosestEnemy FAILED: TurnManager is NULL. (Did BasePawn.h declare its own TurnManager?)"));
		return nullptr;
	}

	// --- 2. 내(Player)가 Ally인지 확인 ---
	if (!bIsAlly)
	{
		UE_LOG(LogTemp, Error, TEXT("FindClosestEnemy FAILED: Player's (My) bIsAlly is FALSE. (Looking in AllyUnits instead of EnemyUnits)"));
	}

	// --- 3. 적 목록 가져오기 ---
	TArray<ATurnBasedUnit*>& EnemyList = (bIsAlly) ? GetTurnManager()->EnemyUnits : GetTurnManager()->AllyUnits;
	if (EnemyList.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("FindClosestEnemy FAILED: EnemyList is EMPTY. (Did BP_PawnTurret set bIsAlly to FALSE?)"));
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("FindClosestEnemy: Found %d potential enemies."), EnemyList.Num());

	ATurnBasedUnit* ClosestEnemy = nullptr;
	float MinDistSq = MAX_FLT;

	for (ATurnBasedUnit* Enemy : EnemyList)
	{
		if (!Enemy) continue; // 배열에 null 포인터가 있으면 건너뛰기

		// --- 4. 상속(캐스팅) 확인 ---
		ABasePawn* EnemyPawn = Cast<ABasePawn>(Enemy);
		if (!EnemyPawn)
		{
			UE_LOG(LogTemp, Error, TEXT("FindClosestEnemy SKIPPED target: '%s' does not inherit from ABasePawn."), *Enemy->GetName());
			continue;
		}

		// --- 5. 체력 확인 ---
		if (EnemyPawn->Health <= 0.0f)
		{
			UE_LOG(LogTemp, Error, TEXT("FindClosestEnemy SKIPPED target: '%s' has 0 or less Health."), *EnemyPawn->GetName());
			continue;
		}

		// --- 6. 유효한 타겟 발견 및 거리 계산 ---
		float DistSq = FVector::DistSquared(GetActorLocation(), Enemy->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestEnemy = Enemy;
		}
	}

	if (!ClosestEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("FindClosestEnemy FAILED: All enemies were SKIPPED by filters (Cast failed or Health=0)."));
	}

	return ClosestEnemy;
}
// 조준 및 발사 함수 (ProcessNextAction에서 호출)
void ABasePawn::AimAndFire(FVector TargetLocation, bool bWillDealDamage)
{
	AimTargetLocation = TargetLocation;
	bShotWillDealDamage = bWillDealDamage;
	bIsAiming = true; // Tick 함수가 이 상태를 감지하고 포탑을 회전시킴
}

void ABasePawn::Fire(bool bShouldDealDamage) {
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	if (Projectile) {
		Projectile->SetOwner(this);
		Projectile->bShouldDealDamage = bShouldDealDamage;
		Projectile->DamageToDeal = AttackDamage; // Projectile에게 데미지 전달
	}

}

