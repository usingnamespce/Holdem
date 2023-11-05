// Copyright Epic Games, Inc. All Rights Reserved.

#include "HoldemGameMode.h"
#include "HoldemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHoldemGameMode::AHoldemGameMode()
{
	
}

FCardInfo AHoldemGameMode::GetUnallocatedCard()
{
	int32 RandomCardID = -1;
	while(true)
	{
		RandomCardID = FMath::RandRange(0,51);
		if(!HasAllocatedCards.Contains(RandomCardID))
		{
			HasAllocatedCards.Add(RandomCardID);
			break;
		}
	}
	FCardInfo NewCardInfo = AHoldemCard::CardIdToCardInfo(RandomCardID);
	UE_LOG(LogTemp,Log,TEXT("获得未分配的卡牌：卡牌ID:%d   卡牌类型：%s   卡牌大小:%d"),NewCardInfo.CardID,*UEnum::GetValueAsString(NewCardInfo.Type),NewCardInfo.CardNumber);
	return NewCardInfo;
}

void AHoldemGameMode::ResetCardPool()
{
	HasAllocatedCards.Reset();
}

ECompareResult AHoldemGameMode::CompareCards(FCardsKeyInfo ACards, FCardsKeyInfo BCards)
{
	// 大于
	if(ACards.CardsType > BCards.CardsType)
	{
		return ECompareResult::Greater;
	}
	// 小于
	if(ACards.CardsType < BCards.CardsType)
	{
		return ECompareResult::Less;
	}
	// 比较四条
	if(ACards.CardsType == ECardsType::FourKind)
	{
		const int32 ACardsKeyNum = *ACards.CardsNum.FindKey(4);
		const int32 BCardsKeyNum = *BCards.CardsNum.FindKey(4);
		return ACardsKeyNum > BCardsKeyNum ? ECompareResult::Greater : ECompareResult::Less;
	}
	// 比较同花
	if(ACards.CardsType == ECardsType::Flush)
	{
		// 其实就是比较高牌
		return CompareHigh(ACards,BCards);
	}
	// 比较顺子
	if(ACards.CardsType == ECardsType::Straight)
	{	
		return CompareStraight(ACards,BCards);	
	}
	// 比较同花顺
	if(ACards.CardsType == ECardsType::FlushStraight)
	{
		// 其实就是比较顺子
		return CompareStraight(ACards,BCards);
	}
	// 比较葫芦
	if(ACards.CardsType == ECardsType::FullHouse)
	{
		// 其实就是比较三条
		const int32 ACardsKeyNum = *ACards.CardsNum.FindKey(3);
		const int32 BCardsKeyNum = *BCards.CardsNum.FindKey(3);
		return ACardsKeyNum > BCardsKeyNum ? ECompareResult::Greater : ECompareResult::Less;
	}
	// 比较高牌
	if(ACards.CardsType == ECardsType::HighCard)
	{
		return CompareHigh(ACards,BCards);
	}
	// 比较一对
	if(ACards.CardsType == ECardsType::OnPair)
	{
		return ComparePair(ACards,BCards);
	}
	// 比较三条
	if(ACards.CardsType == ECardsType::ThreeKind)
	{
		const int32 ACardsKeyNum = *ACards.CardsNum.FindKey(3);
		const int32 BCardsKeyNum = *BCards.CardsNum.FindKey(3);
		return ACardsKeyNum > BCardsKeyNum ? ECompareResult::Greater : ECompareResult::Less;
	}
	// 比较两对
	if(ACards.CardsType == ECardsType::TwoPair)
	{
		return ComparePair(ACards,BCards);
	}
	
	return ECompareResult::None;
}

bool AHoldemGameMode::IsFlush(TArray<FCardInfo> Cards)
{
	for(int32 i=1;i<=4;i++)
	{
		if(Cards[0].Type != Cards[i].Type)
		{
			return false;
		}
	}
	return true;
}

bool AHoldemGameMode::IsStraight(TArray<FCardInfo> Cards)
{
	for(int32 i=1;i<=4;i++)
	{
		// 保证递减
		if(Cards[i-1].CardNumber != Cards[i].CardNumber + 1)
		{
			// 防止出现A5432
			if(i == 1 && Cards[1].CardNumber == 5 && Cards[0].CardNumber == 14)
			{
				continue;
			}
			return false;
		}
	}
	return true;
}

TMap<int32, int32> AHoldemGameMode::CountCardNum(TArray<FCardInfo> Cards)
{
	TMap<int32, int32> CardsNum;
	for(const FCardInfo& CurCard : Cards)
	{
		int32& CurCardNum = CardsNum.FindOrAdd(CurCard.CardNumber);
		CurCardNum++;
	}
	return CardsNum;
}

ECompareResult AHoldemGameMode::CompareStraight(FCardsKeyInfo ACards, FCardsKeyInfo BCards)
{
	// 首先判断是否为最小的顺子 A5432
	bool bAIsLess = (ACards.CardInfos[1].CardNumber == 5 && ACards.CardInfos[0].CardNumber == 14);
	bool bBIsLess = (BCards.CardInfos[1].CardNumber == 5 && BCards.CardInfos[0].CardNumber == 14);
	
	if(bAIsLess)
	{
		// A√ B√ 
		if(bBIsLess)
		{
			// 比较花色
			return CompareType(ACards,BCards);
		}
		// A√ B×
		return ECompareResult::Less;
	}

	// A× B√
	if(bBIsLess)
	{
		return ECompareResult::Greater;
	}

	// A× B× 其实就是比较第一张牌的大小（若相等，则存在冗余的四次比较，但是懒得优化了）
	return CompareHigh(ACards,BCards);
}

ECompareResult AHoldemGameMode::CompareHigh(FCardsKeyInfo ACards, FCardsKeyInfo BCards)
{
	// 比较牌的大小
	for(int32 i=0;i<5;i++)
	{
		if(ACards.CardInfos[i].CardNumber == BCards.CardInfos[i].CardNumber)
		{
			continue;
		}
		return (ACards.CardInfos[i].CardNumber > BCards.CardInfos[i].CardNumber) ? ECompareResult::Greater : ECompareResult::Less;
	}

	// 比较花色
	return CompareType(ACards,BCards);
}

ECompareResult AHoldemGameMode::CompareType(FCardsKeyInfo ACards, FCardsKeyInfo BCards)
{
	// 比较第一张牌的花色
	if(ACards.CardInfos[0].Type == BCards.CardInfos[0].Type)
	{
		UE_LOG(LogTemp,Warning,TEXT("出现两张花色大小一致的牌"));
		return ECompareResult::None;
	}
	return (ACards.CardInfos[0].Type > BCards.CardInfos[0].Type) ? ECompareResult::Greater : ECompareResult::Less;
}

ECompareResult AHoldemGameMode::ComparePair(FCardsKeyInfo ACards, FCardsKeyInfo BCards)
{
	// 筛选对子
	auto FilterPair = [](TPair<int32, int32> Item)
	{
		return Item.Value == 2;
	};
	
	TArray<int32> ATemp;
	ACards.CardsNum.FilterByPredicate(FilterPair).GenerateKeyArray(ATemp);
	ATemp.Sort([](const int32& A, const int32& B)
	{
		return A > B;
	});
	
	TArray<int32> BTemp;
	BCards.CardsNum.FilterByPredicate(FilterPair).GenerateKeyArray(BTemp);
	BTemp.Sort([](const int32& A, const int32& B)
	{
		return A > B;
	});

	// 2对 > 1对
	if(ATemp.Num() > BTemp.Num())
	{
		return ECompareResult::Greater;
	}
	if(ATemp.Num() < BTemp.Num())
	{
		return ECompareResult::Less;
	}

	// 对数相同
	for(int i=0;i<ATemp.Num();i++)
	{
		if(ATemp[i] > BTemp[i])
		{
			return ECompareResult::Greater;
		}
		if(ATemp[i] < BTemp[i])
		{
			return ECompareResult::Less;
		}
	}

	// 对子都相同了，只有比较最后一张的大小了
	int32 ATempNum = *ACards.CardsNum.FindKey(1);
	int32 BTempNum = *BCards.CardsNum.FindKey(1);
	if(ATempNum > BTempNum)
	{
		return ECompareResult::Greater;
	}
	if(ATempNum < BTempNum)
	{
		return ECompareResult::Less;
	}
	// 比较花色
	return CompareType(ACards,BCards);
}

FCardsKeyInfo AHoldemGameMode::AnalyzeCardsKeyInfo(TArray<FCardInfo> Cards)
{
	if(Cards.Num() != 5)
	{
		UE_LOG(LogTemp,Warning,TEXT("牌组牌数牌数不为5"));
		return FCardsKeyInfo();
	}
	
	// 首先对牌组进行排序（大的在前）
	auto CompareLambda = [](const FCardInfo& A, const FCardInfo& B)
	{
		// 比较大小
		if(A.CardNumber > B.CardNumber)
		{
			return true;
		}

		// 大小相等比较花色
		if(A.CardNumber == B.CardNumber)
		{
			return A.Type > B.Type;
		}
		
		return false;
	};
	Cards.Sort(CompareLambda);
	
	TMap<int32,int32> CardsNum = CountCardNum(Cards);
	
	FCardsKeyInfo CardsKeyInfo;
	CardsKeyInfo.CardsNum = CardsNum;
	CardsKeyInfo.CardsType = ECardsType::HighCard;
	CardsKeyInfo.CardInfos = Cards;

	//判断是否为 同花or顺子
	if(IsFlush(Cards))
	{
		CardsKeyInfo.CardsType = ECardsType::Flush;
		if(IsStraight(Cards))
		{
			CardsKeyInfo.CardsType = ECardsType::FlushStraight;
		}
	}
	else if(IsStraight(Cards))
	{
		CardsKeyInfo.CardsType = ECardsType::Straight;
	}
	
	// 如果只有2个---->四条or葫芦
	if(CardsNum.Num() == 2)
	{
		for(const auto& CardNum : CardsNum)
		{
			if(CardNum.Value == 4 || CardNum.Value == 1)
			{
				CardsKeyInfo.CardsType = ECardsType::FourKind;
			}
			else
			{
				CardsKeyInfo.CardsType = ECardsType::FullHouse;
			}
			break;
		}
	}
	// 如果有3个---->三条or两对
	else if(CardsNum.Num() == 3)
	{
		CardsKeyInfo.CardsType = ECardsType::TwoPair;
		for(const auto& CardNum : CardsNum)
		{
			if(CardNum.Value == 3)
			{
				CardsKeyInfo.CardsType = ECardsType::ThreeKind;
			}
		}
	}
	// 如果有4个---->一对
	else if(CardsNum.Num() == 4)
	{
		CardsKeyInfo.CardsType = ECardsType::OnPair;
	}
	
	return CardsKeyInfo;
}
