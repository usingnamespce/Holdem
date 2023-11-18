// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HoldemPlayerState.h"
#include "HoldemPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnDelegate);

USTRUCT(BlueprintType)
struct FPlayerTurnInfo
{
	GENERATED_BODY()

	// 玩家
	UPROPERTY(BlueprintReadWrite)
	AHoldemPlayerState* Player;

	// 增加的筹码
	UPROPERTY(BlueprintReadWrite)
	int32 Chips;

	// 是否All-in
	UPROPERTY(BlueprintReadWrite)
	bool bIsAllIn;

	// 是否弃牌
	UPROPERTY(BlueprintReadWrite)
	bool bIsFold;
};

/**
 * 
 */
UCLASS()
class HOLDEM_API AHoldemPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 我的回合
	UFUNCTION(BlueprintCallable,Client,Reliable)
	void OnMyTurn();
	// 新的游戏
	UFUNCTION(BlueprintCallable,Client,Reliable)
	void OnNewGame();
	// 结束游戏
	UFUNCTION(BlueprintCallable,Client,Reliable)
	void OnEndGame();
	// 确认我的回合
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void EnsureMyTurn(FPlayerTurnInfo PlayerTurnInfo);
	// 准备游戏
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void PlayerReady(int32 PlayerRemainChips);

public:
	UPROPERTY(BlueprintAssignable)
	FTurnDelegate OnMyTurnDelegate;

	UPROPERTY(BlueprintAssignable)
	FTurnDelegate OnNewGameDelegate;

	UPROPERTY(BlueprintAssignable)
	FTurnDelegate OnEndGameDelegate;
};
