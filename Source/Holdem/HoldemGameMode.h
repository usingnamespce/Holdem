// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldemCard.h"
#include "GameFramework/GameModeBase.h"
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
};

UCLASS(minimalapi)
class AHoldemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHoldemGameMode();

	//获得未被分发的一张卡牌
	UFUNCTION(BlueprintPure)
	FCardInfo GetUnallocatedCard();

	//重置卡牌池子
	UFUNCTION(BlueprintCallable)
	void ResetCardPool();

	// 通过牌组的关键信息，比较两个牌组的大小
	UFUNCTION(BlueprintPure)
	static ECompareResult CompareCards(FCardsKeyInfo ACards, FCardsKeyInfo BCards);
	
	// 分析牌组的关键信息
	UFUNCTION(BlueprintPure)
	static FCardsKeyInfo AnalyzeCardsKeyInfo(TArray<FCardInfo> Cards);

	// 选出玩家的最佳组合牌型, 若为空值代表全部用的是公共牌
	UFUNCTION(BlueprintCallable)
	TArray<FCardInfo> ChoosePlayerBestCards(AHoldemPlayerState* Player);
	
private:
	// 判断是否同花
	static bool IsFlush(TArray<FCardInfo> Cards);
	// 判断是否为顺子 --需要排序后的牌组（大的在前）
	static bool IsStraight(TArray<FCardInfo> Cards);
	// 对牌组的牌数进行统计 --需要排序后的牌组（大的在前）
	static TMap<int32,int32> CountCardNum(TArray<FCardInfo> Cards);
	// 比较顺子的大小
	static ECompareResult CompareStraight(FCardsKeyInfo ACards, FCardsKeyInfo BCards);
	// 比较高牌
	static ECompareResult CompareHigh(FCardsKeyInfo ACards, FCardsKeyInfo BCards);
	// 比较花色
	static ECompareResult CompareType(FCardsKeyInfo ACards, FCardsKeyInfo BCards);
	// 比较对子
	static ECompareResult ComparePair(FCardsKeyInfo ACards, FCardsKeyInfo BCards);
	
public:
	// 桌子的引用
	UPROPERTY(BlueprintReadOnly)
	AHoldemDesk* HoldemDesk;

	// 已分配的卡牌
	UPROPERTY(BlueprintReadWrite)
	TArray<int32> HasAllocatedCards;

	TArray<TArray<int32>> CombinationMode;
};



