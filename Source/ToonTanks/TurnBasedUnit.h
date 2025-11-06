#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnActionTypes.h"
#include "Containers/Queue.h"
#include "GridManager.h"

#include "TurnBasedUnit.generated.h"

class ATurnManager;

UCLASS()
class TOONTANKS_API ATurnBasedUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATurnBasedUnit();

	///// ---------- 변수

	// 이 유닛이 현재 턴을 가지고 있는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	bool bIsMyTurn = false;

	// 유닛의 아군/적 상태를 에디터에서 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	bool bIsAlly = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FIntPoint CurrentGridCoordinate;

	////// ---------- 함수

	// GameMode가 유닛을 스폰한 직후 호출할 초기화 함수
	void Initialize(FIntPoint StartCoordinate);

	// 부모 클래스는 'virtual' 키워드만 사용하여 이 함수들이 재정의될 수 있음을 알림
	// 턴이 시작되었을 때 TurnManager가 호출할 함수
	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void OnTurnStarted();
	virtual void OnTurnStarted_Implementation();

	// 턴이 종료되었을 때 호출될 함수
	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void OnTurnEnded();
	virtual void OnTurnEnded_Implementation();

	UFUNCTION(BlueprintCallable, Category = "UI")
	// 행동 제출 함수
	void ExecuteActionQueue();

	// 턴을 완전히 종료하고 TurnManager에게 알리는 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void EndTurn();

private:
	//// 실제 이동을 수행하는 내부 함수
	//void AttemptMove(FIntPoint TargetCoordinate);

protected:

	// ----- 변수

	// 모든 유닛이 TurnManager에 쉽게 접근할 수 있도록 참조 저장
	UPROPERTY()
	ATurnManager* TurnManager;

	ATurnManager* GetTurnManager();

	// 턴마다 주어지는 최대 행동력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn System")
	int32 MaxActionPoints = 5;

	// 현재 남은 행동력(큐에 추가할 때마다 차감)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	int32 CurrentActionPoints;

	// 수행할 행동을 순서대로 저장하는 큐
	TQueue<EUnitAction> ActionQueue;

	// 현재 큐의 행동을 실행 중인지 여부 (중복 입력 방지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	bool bIsExecutingActions = false;

	// 이동이 진행 중인지 여부
	bool bIsMoving = false;

	// 이동할 목표의 월드 위치
	FVector SmoothMoveTargetLocation;

	// 부드러운 회전 목표값 (Yaw)
	FRotator SmoothMoveTargetRotation;

	// 이동 속도
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationSpeed = 10.0f;

	UPROPERTY()
	AGridManager* GridManagerRef;

	// ----- 함수

	// 큐에 쌓인 action 처리 함수
	void ProcessNextAction();

	// 기존 이동 함수들 입력 바인드용으로 래핑
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddMoveUpAction();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddMoveDownAction();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddMoveLeftAction();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddMoveRightAction();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddAttack();

	void CommonActionsOnBinding();

	// 실제 이동 로직(bool을 반환하여 성공/실패를 알림)
	bool AttemptMove(FIntPoint TargetCoordinate);

	// 자식 클래스가 재정의할 수 있는 가상 공격 함수	
	virtual void HandleAttackAction();

	// AI 유닛이 턴 시작 시 호출할 함수
	void ExecuteEnemyAI();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
