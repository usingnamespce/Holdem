// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldemCard.h"
#include "GameFramework/GameModeBase.h"
#include "HoldemGameMode.generated.h"

class AHoldemDesk;

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

	// 比较两个牌的大小 1->大 0->无法比较 -1->小
	UFUNCTION(BlueprintPure)
	static int32 CompareCards(TArray<FCardInfo> ACards, TArray<FCardInfo> BCards);
	
	// 分析牌组的关键信息
	UFUNCTION(BlueprintPure)
	static FCardsKeyInfo AnalyzeCardsKeyInfo(TArray<FCardInfo> Cards);
	
private:
	// 判断是否同花
	static bool IsFlush(TArray<FCardInfo> Cards);
	// 判断是否为顺子 --需要排序后的牌组（大的在前）
	static bool IsStraight(TArray<FCardInfo> Cards);
	// 对牌组的牌数进行统计 --需要排序后的牌组（大的在前）
	static TMap<int32,int32> CountCardNum(TArray<FCardInfo> Cards);
	
public:
	// 桌子的引用
	UPROPERTY(BlueprintReadOnly)
	AHoldemDesk* HoldemDesk;

	// 已分配的卡牌
	UPROPERTY(BlueprintReadWrite)
	TArray<int32> HasAllocatedCards;
};



