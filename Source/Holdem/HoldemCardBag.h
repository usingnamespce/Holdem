// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoldemCardBag.generated.h"

class AHoldemPlayerState;

UCLASS()
class HOLDEM_API AHoldemCardBag : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AHoldemCardBag();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnOwnerCardChange();
private:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOwnerCardChange();

public:
	UPROPERTY(BlueprintReadOnly)
	AHoldemPlayerState* OwnerPlayerState;
};
