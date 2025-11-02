#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnBasedUnit.generated.h"
#include "TurnActionTypes.h"
#include "Containers/Queue.h"

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

	// 유닛이 이동이나 공격 같은 행동을 수행하는 함수
	void PerformAction();

	// GameMode가 유닛을 스폰한 직수 호출할 초기화 함수
	void Initialize(FIntPoint StartCoordinate);

	// 턴이 시작되었을 때 TurnManager가 호출할 함수
	virtual void OnTurnStarted();

	// 턴이 종료되었을 때 호출될 함수
	virtual void OnTurnEnded();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveUp();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveDown();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveLeft();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveRight();

private:
	//// 실제 이동을 수행하는 내부 함수
	//void AttemptMove(FIntPoint TargetCoordinate);

protected:
	// 모든 유닛이 TurnManager에 쉽게 접근할 수 있도록 참조 저장
	UPROPERTY()
	ATurnManager* TurnManager;

	// 턴마다 주어지는 최대 행동력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn System")
	int32 MaxActionPoints = 3;

	// 현재 남은 행동력(큐에 추가할 때마다 차감)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	int32 CurrentActionPoints;

	// 수행할 행동을 순서대로 저장하는 큐
	TQueue<EUnitAction> ActionQueue;

	// 현재 큐의 행동을 실행 중인지 여부 (중복 입력 방지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	bool bIsExecutingActions = false;

	// 큐에 쌓인 action 처리 함수
	void ProcessNextAction();

	// 턴을 완전히 종료하고 TrunManager에게 알리는 함수
	void EndTurn();

	// 기존 이동 함수들 입력 바인등요으로 래핑
	void AddMoveUpAction();
	void AddMoveDownAction();
	void AddMoveLeftAction();
	void AddMoveRightAction();

	// 실제 이동 로직(bool을 반환하여 성공/실패를 알림)
	bool AttemptMove(FIntPoint TargetCoordinate);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
