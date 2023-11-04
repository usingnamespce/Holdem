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

int32 AHoldemGameMode::CompareCards(TArray<FCardInfo> ACards, TArray<FCardInfo> BCards)
{
	
	if(ACards.Num() != 5 && BCards.Num() != 5)
	{
		UE_LOG(LogTemp,Warning,TEXT("比较大小，但是牌数不为5"));
		return 0;
	}

	// 判断是否同花
	return -1;
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

FCardsKeyInfo AHoldemGameMode::AnalyzeCardsKeyInfo(TArray<FCardInfo> Cards)
{
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
