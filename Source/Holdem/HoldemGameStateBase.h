// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HoldemCard.h"
#include "HoldemGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class HOLDEM_API AHoldemGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_PublicCards)
	TArray<FCardInfo> PublicCards;
	
private:
	
public:
	
	UFUNCTION(BlueprintCallable)
	void AddPublicCard(FCardInfo NewCard);

	UFUNCTION(BlueprintCallable)
	void ResetPublicCardsPool();
	
private:
	UFUNCTION()
	void OnRep_PublicCards();
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
};
