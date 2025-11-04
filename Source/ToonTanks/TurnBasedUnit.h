#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnActionTypes.h"
#include "Containers/Queue.h"
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

	// 유닛이 이동이나 공격 같은 행동을 수행하는 함수
	void PerformAction();

	// GameMode가 유닛을 스폰한 직수 호출할 초기화 함수
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


	// 실제 이동 로직(bool을 반환하여 성공/실패를 알림)
	bool AttemptMove(FIntPoint TargetCoordinate);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ----- UI 관련 변수 -----
	// 유닛이 사용할 UI 위젯 클래스, BP에서 설정
	//UPROPERTY(EditDefaultsOnly, Category = "UI")
	//TSubclassOf<UUserWidget> ActionWidgetClass;

	// BeginPlay에서 생성된 ActionWidget의 실제 인스턴스
	// 턴마다 재사용을 위한 저장
	//UPROPERTY()
	//UUserWidget* ActionWidgetInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
