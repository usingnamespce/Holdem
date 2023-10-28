// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HoldemGameMode.generated.h"

class AHoldemDesk;

UCLASS(minimalapi)
class AHoldemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHoldemGameMode();
	
public:
	UPROPERTY(BlueprintReadOnly)
	AHoldemDesk* HoldemDesk;
};



