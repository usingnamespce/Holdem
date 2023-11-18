// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldemCard.h"
#include "GameFramework/GameModeBase.h"
#include "HoldemPlayerController.h"
#include "HoldemGameMode.generated.h"

class AHoldemDesk;
class AHoldemPlayerState;

UENUM(BlueprintType)
enum class ECompareResult : uint8
{
	//无法比较
	None,
	//相等
	Equality,
	//大于
	Greater,
	//小于
	Less,
};

// 牌组牌型
UENUM(BlueprintType)
enum class ECardsType : uint8
{
	//高牌
	HighCard,
	//一对
	OnPair,
	//两对
	TwoPair,
	//三条
	ThreeKind,
	//顺子
	Straight,
	//同花
	Flush,
	//葫芦
	FullHouse,
	//四条
	FourKind,
	//同花顺
	FlushStraight,
};

// 卡牌组的关键信息
USTRUCT(BlueprintType)
struct FCardsKeyInfo
{
	GENERATED_BODY()

	//牌组对应牌的数量
	UPROPERTY(BlueprintReadOnly)
	TMap<int32,int32> CardsNum;

	//牌组的牌型
	UPROPERTY(BlueprintReadOnly)
	ECardsType CardsType;

	// 排序后的牌组原始信息
	UPROPERTY(BlueprintReadOnly)
	TArray<FCardInfo> CardInfos;

	// 判断信息是否有效
	bool IsValid() const 
	{
		if(CardsNum.Num() == 0)
		{
			return false;
		}

		if(CardInfos.Num() == 0)
		{
			return false;
		}
		
		return true;
	}

	bool operator== (const FCardsKeyInfo& Other) const
	{
		if(CardsType != Other.CardsType)
		{
			return false;
		}
		if(CardsNum.Num() != Other.CardsNum.Num())
		{
			return false;
		}
		if(CardInfos != Other.CardInfos)
		{
			return false;
		}
		return true;
	}
};

// 玩家一回合游戏的牌组信息
USTRUCT(BlueprintType)
struct FPlayerGameCardInfo
{
	GENERATED_BODY()

	//玩家
	UPROPERTY(BlueprintReadOnly)
	AHoldemPlayerState* Player;

	//玩家牌组关键信息
	UPROPERTY(BlueprintReadOnly)
	FCardsKeyInfo CardsKeyInfo;
};

// 玩家一回合游戏的状态信息
USTRUCT(BlueprintType)
struct FPlayerGameStateInfo
{
	GENERATED_BODY()

	// 所加筹码大小
	UPROPERTY(BlueprintReadWrite)
	int32 Chip;

	// 玩家
	UPROPERTY(BlueprintReadOnly)
	AHoldemPlayerState* Player;

	// index
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerIndex;

	// 是否弃牌
	UPROPERTY(BlueprintReadWrite)
	bool bIsFold;

	// 是否All-in
	UPROPERTY(BlueprintReadWrite)
	bool bIsAllIn;
};

UCLASS(minimalapi)
class AHoldemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHoldemGameMode();

	// 获得未被分发的一张卡牌
	UFUNCTION(BlueprintPure)
	FCardInfo GetUnallocatedCard();

	// 重置卡牌池子
	UFUNCTION(BlueprintCallable)
	void ResetCardPool();

	// 通过牌组的关键信息，比较两个牌组的大小
	UFUNCTION(BlueprintPure)
	static ECompareResult CompareCards(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards);
	
	// 分析牌组的关键信息
	UFUNCTION(BlueprintPure)
	static FCardsKeyInfo AnalyzeCardsKeyInfo(TArray<FCardInfo> Cards);

	// 选出玩家的最佳组合牌型
	UFUNCTION(BlueprintCallable)
	FCardsKeyInfo ChoosePlayerBestCards(AHoldemPlayerState* Player);

	// 分析玩家的牌组信息
	UFUNCTION(BlueprintCallable)
	TArray<FPlayerGameCardInfo> AnalyzeAllPlayerCards(TArray<AHoldemPlayerState*> Players);

	// 开始新对局
	UFUNCTION(BlueprintCallable)
	void StartNewGame();

	// 尝试开始新对局
	UFUNCTION(BlueprintCallable)
	bool TryStartNewGame();

	// 准备开始新对局
	UFUNCTION(BlueprintCallable)
	void PrepareStartNewGame();

	// 对局结算
	UFUNCTION(BlueprintCallable)
	void SettleGame();

	// 是否可以进入下一回合
	UFUNCTION(BlueprintPure)
	bool CanEnterNextRound();

	// 尝试进入下一回合
	UFUNCTION(BlueprintCallable)
	void TryEnterNextRound();

	// 结束游戏
	UFUNCTION(BlueprintCallable)
	void OnEndGame();

	// 蓝图结束游戏
	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="OnEndGame"))
	void K2_OnEndGame();

	// 当玩家确认回合
	UFUNCTION()
	void OnPlayerEnsureTurn(const FPlayerTurnInfo& PlayerTurnInfo);

	// 当玩家准备游戏
	UFUNCTION()
	void OnPlayerReady(const int32& ReaminChips, AHoldemPlayerState* Player);

	// 通知玩家开始回合
	UFUNCTION(BlueprintCallable)
	bool NotifyPlayerStartTurn();

	// 统计有效玩家
	int32 CountValidPlayers() const;
	
private:
	// 判断是否同花
	static bool IsFlush(TArray<FCardInfo> Cards);
	// 判断是否为顺子 --需要排序后的牌组（大的在前）
	static bool IsStraight(TArray<FCardInfo> Cards);
	// 对牌组的牌数进行统计 --需要排序后的牌组（大的在前）
	static TMap<int32,int32> CountCardNum(TArray<FCardInfo> Cards);
	// 比较顺子的大小
	static ECompareResult CompareStraight(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards);
	// 比较高牌
	static ECompareResult CompareHigh(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards);
	// 比较花色
	static ECompareResult CompareType(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards);
	// 比较对子
	static ECompareResult ComparePair(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards);
	// 平均分配筹码给玩家
	static void EvenlyDistributeChips(TArray<AHoldemPlayerState*> Players, int32 Chips);

	// 计算玩家边池
	void CalPlayersSidePot();
	
public:
	// 桌子的引用
	UPROPERTY(BlueprintReadOnly)
	AHoldemDesk* HoldemDesk;

	// 已分配的卡牌
	UPROPERTY(BlueprintReadOnly)
	TArray<int32> HasAllocatedCards;
	
	// 对应玩家的边池
	UPROPERTY(BlueprintReadOnly)
	TMap<AHoldemPlayerState*,int32> PlayerSidePot;
	
private:
	TArray<TArray<int32>> CombinationMode;

	// 用于计算玩家边池, Value为玩家投入的总筹码
	UPROPERTY()
	TMap<AHoldemPlayerState*,int32> TempPlayersSidePot;

	int32 ReadyPlayersNum;

	bool bGameIsReady;
};



