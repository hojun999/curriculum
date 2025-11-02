// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EUnitAction : uint8 {
	MoveUp,
	MoveDown,
	MoveLeft,
	MoveRight,
	Attack
};

class TOONTANKS_API TurnActionTypes
{
public:
	TurnActionTypes();
	~TurnActionTypes();
};
