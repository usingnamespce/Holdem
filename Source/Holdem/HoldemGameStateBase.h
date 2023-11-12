// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HoldemGameMode.h"
#include "HoldemCard.h"
#include "HoldemGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EHoldemGameState : uint8
{
	Blinds		UMETA(DisplayName = "盲注"),
	PreFlop		UMETA(DisplayName = "翻牌前"),
	Flop		UMETA(DisplayName = "翻牌"),
	Turn		UMETA(DisplayName = "转牌"),
	River		UMETA(DisplayName = "河牌"),
	Showdown	UMETA(DisplayName = "摊牌")
};

UCLASS()
class HOLDEM_API AHoldemGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_PublicCards)
	TArray<FCardInfo> PublicCards;

	// 玩家的对局信息
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_PlayerGameStateInfos)
	TArray<FPlayerGameStateInfo> PlayerGameStateInfos;

	// 当前回合
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_HoldemGameState)
	EHoldemGameState HoldemGameState;

	// 底池大小
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Pot)
	int32 Pot;

	// 当前回合的玩家Index
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_CurrentPlayerIndex)
	int32 CurrentPlayerIndex;

	// 盲注最低筹码
	UPROPERTY(BlueprintReadWrite,Replicated,EditDefaultsOnly)
	int32 BlindMinChips;

	// 用于计算盲注进度
	UPROPERTY(BlueprintReadOnly,Replicated)
	int32 BlindsCount = 0;
private:

	
	
public:
	
	UFUNCTION(BlueprintCallable)
	void AddPublicCard(FCardInfo NewCard);

	UFUNCTION(BlueprintCallable)
	void ResetPublicCardsPool();

	UFUNCTION(BlueprintCallable)
	void OnNewGame();

	// 获得玩家所需加注的最大筹码
	UFUNCTION(BlueprintPure)
	int32 GetMaxChips(AHoldemPlayerState* Player) const;

	AHoldemGameStateBase();
	
private:
	UFUNCTION()
	void OnRep_PublicCards();
	UFUNCTION()
	void OnRep_PlayerGameStateInfos();
	UFUNCTION()
	void OnRep_HoldemGameState();
	UFUNCTION()
	void OnRep_Pot();
	UFUNCTION()
	void OnRep_CurrentPlayerIndex();
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
};
