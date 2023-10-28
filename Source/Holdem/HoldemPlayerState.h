// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HoldemCard.h"
#include "HoldemDesk.h"
#include "HoldemPlayerState.generated.h"

class AHoldemCardBag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardsChangeDelegate);

UCLASS()
class HOLDEM_API AHoldemPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerCards)
	TArray<FCardInfo> PlayerCards;

	UPROPERTY(Replicated)
	AHoldemCardBag* CardBag;

	UPROPERTY(Replicated)
	FPlayerLocationInfo PlayerLocationInfo;
	
public:
	// 玩家拥有卡牌数量变化时的委托
	UPROPERTY(BlueprintAssignable)
	FCardsChangeDelegate OnPlayerCardChange;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHoldemCardBag> CardBagClass;

private:
	UFUNCTION()
	void OnRep_PlayerCards();
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void AddCard(FCardInfo NewCardInfo);

	UFUNCTION(BlueprintPure)
	TArray<FCardInfo> GetCardsInfo();

	// 设置玩家位置信息, 并Spawn 卡牌背包
	UFUNCTION(BlueprintCallable)
	void SetPlayerLocationInfo(FPlayerLocationInfo InPlayerLocationInfo);
};
