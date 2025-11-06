// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnBasedUnit.h"
#include "BasePawn.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class USceneComponent;
class UFloatingPawnMovement;
class AProjectile;

UCLASS()
class TOONTANKS_API ABasePawn : public ATurnBasedUnit
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 400.f;
	
	void HandleDestruction();

	virtual void OnTurnStarted_Implementation() override;
	virtual void OnTurnEnded_Implementation() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	ATurnManager* TurnManager;

	virtual void Tick(float DeltaTime) override;

	void BeginPlay() override;

	// 공격 큐를 처리할 때 호출되는 함수(ProcessNextAction에서 호출)
	virtual void HandleAttackAction() override;

	// 가장 가까운 적 유닛을 찾는 함수
	ATurnBasedUnit* FindClosestEnemy();

	// 포탑 회전 및 발사 로직
	void AimAndFire(FVector TargetLocation, bool bWillDealDamage);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Fire(bool bShouldDealDamage);

	// ----- 전투 관련 변수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Health = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxAttackRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 10.0f;

	// 조준 상태 변수
	bool bIsAiming = false; // 현재 조준 중인지 여부
	FVector AimTargetLocation; // 조준할 목표의 월드 위치
	bool bShotWillDealDamage = false; // 이 공격이 데미지를 입힐지 여부 (Projectile에게 전달)

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* FloatingMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UParticleSystem* DeathParticles;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class UCameraShakeBase> DeathCameraShakeClass;
};
