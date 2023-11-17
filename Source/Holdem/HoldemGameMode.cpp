// Copyright Epic Games, Inc. All Rights Reserved.

#include "HoldemGameMode.h"
#include "HoldemCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "HoldemGameStateBase.h"
#include "HoldemPlayerController.h"
#include "HoldemPlayerState.h"

AHoldemGameMode::AHoldemGameMode()
{
	CombinationMode.Append({{ 0,1,2,3,4 }, { 0,1,2,3,5 }, { 0,1,2,3,6 }, 
						    { 0,1,2,4,5 }, { 0,1,2,4,6 }, { 0,1,2,5,6 }, 
						    { 0,1,3,4,5 }, { 0,1,3,4,6 }, { 0,1,3,5,6 }, 
						    { 0,1,4,5,6 }, { 0,2,3,4,5 }, { 0,2,3,4,6 }, 
						    { 0,2,3,5,6 }, { 0,2,4,5,6 }, { 0,3,4,5,6 }, 
						    { 1,2,3,4,5 }, { 1,2,3,4,6 }, { 1,2,3,5,6 }, 
						    { 1,2,4,5,6 }, { 1,3,4,5,6 }, { 2,3,4,5,6 }});
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
	const FCardInfo NewCardInfo = AHoldemCard::CardIdToCardInfo(RandomCardID);
	UE_LOG(LogTemp,Log,TEXT("获得未分配的卡牌：卡牌ID:%d   卡牌类型：%s   卡牌大小:%d"),NewCardInfo.CardID,*UEnum::GetValueAsString(NewCardInfo.Type),NewCardInfo.CardNumber);
	return NewCardInfo;
}

void AHoldemGameMode::ResetCardPool()
{
	HasAllocatedCards.Reset();
}

ECompareResult AHoldemGameMode::CompareCards(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards)
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

ECompareResult AHoldemGameMode::CompareStraight(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards)
{
	// 首先判断是否为最小的顺子 A5432
	const bool bAIsLess = (ACards.CardInfos[1].CardNumber == 5 && ACards.CardInfos[0].CardNumber == 14);
	const bool bBIsLess = (BCards.CardInfos[1].CardNumber == 5 && BCards.CardInfos[0].CardNumber == 14);
	
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

ECompareResult AHoldemGameMode::CompareHigh(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards)
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

ECompareResult AHoldemGameMode::CompareType(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards)
{
	// 比较第一张牌的花色
	if(ACards.CardInfos[0].Type == BCards.CardInfos[0].Type)
	{
		UE_LOG(LogTemp,Warning,TEXT("出现两张花色大小一致的牌,比较的两组牌相等???"));
		return ECompareResult::Equality;
	}
	return (ACards.CardInfos[0].Type > BCards.CardInfos[0].Type) ? ECompareResult::Greater : ECompareResult::Less;
}

ECompareResult AHoldemGameMode::ComparePair(const FCardsKeyInfo& ACards, const FCardsKeyInfo& BCards)
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
	const int32 ATempNum = *ACards.CardsNum.FindKey(1);
	const int32 BTempNum = *BCards.CardsNum.FindKey(1);
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

void AHoldemGameMode::EvenlyDistributeChips(TArray<AHoldemPlayerState*> Players, int32 Chips)
{
	const int32 Num = Players.Num();
	for(int32 i=0;i<Num;i++)
	{
		Players[i]->RemainChips += Chips / Num;
	}
}

void AHoldemGameMode::CalPlayersSidePot()
{
	const AHoldemGameStateBase* HoldemGameState = GetGameState<AHoldemGameStateBase>();
	for(const auto& Item : TempPlayersSidePot)
	{
		int32& TempChips = PlayerSidePot.FindOrAdd(Item.Key);
		for(const FPlayerGameStateInfo& PlayerGameStateInfo: HoldemGameState->PlayerGameStateInfos)
		{
			TempChips += PlayerGameStateInfo.Chip > Item.Value ? Item.Value : PlayerGameStateInfo.Chip;
		}
	}
	TempPlayersSidePot.Reset();
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

FCardsKeyInfo AHoldemGameMode::ChoosePlayerBestCards(AHoldemPlayerState* Player)
{
	const TArray<FCardInfo> PublicCards = Cast<AHoldemGameStateBase>(UGameplayStatics::GetGameState(this))->PublicCards;
	if (PublicCards.Num() != 5)
	{
		UE_LOG(LogTemp, Error, TEXT("公共牌数不为5"));
		return FCardsKeyInfo();
	}

	// 将公共牌型视为最佳牌型
	FCardsKeyInfo BestCardsKeyInfo = AnalyzeCardsKeyInfo(PublicCards);

	const TArray<FCardInfo> PlayerCards = Player->GetCardsInfo();
	if (PlayerCards.Num() != 2)
	{
		UE_LOG(LogTemp, Error, TEXT("玩家%s牌数不为2"),*Player->GetPlayerName());
		return FCardsKeyInfo();
	}

	// 所有牌
	TArray<FCardInfo> AllCards;
	AllCards.Append(PublicCards);
	AllCards.Append(PlayerCards);

	// 遍历21次得到所有可能牌型
	for(int32 i=0;i<21;i++)
	{
		TArray<FCardInfo> TempCards;
		for(int32 j=0;j<5;j++)
		{
			TempCards.Add(AllCards[CombinationMode[i][j]]);
		}
		// 与最佳牌组进行比较
		const FCardsKeyInfo TempCardKeyInfo = AnalyzeCardsKeyInfo(TempCards);
		const ECompareResult Result = CompareCards(BestCardsKeyInfo,TempCardKeyInfo);
		if(Result == ECompareResult::Less)
		{
			BestCardsKeyInfo = TempCardKeyInfo;
		}
	}
	
	return BestCardsKeyInfo;
}

TArray<FPlayerGameCardInfo> AHoldemGameMode::AnalyzeAllPlayerCards(TArray<AHoldemPlayerState*> Players)
{
	TArray<FPlayerGameCardInfo> PlayerGameInfos;
	for(AHoldemPlayerState* CurPlayerState : Players)
	{
		FPlayerGameCardInfo TempPlayerGameInfo;
		TempPlayerGameInfo.Player = CurPlayerState;
		TempPlayerGameInfo.CardsKeyInfo = ChoosePlayerBestCards(TempPlayerGameInfo.Player);
		PlayerGameInfos.Add(TempPlayerGameInfo);
	}
	// 将玩家游戏信息进行排序（从大到小）
	PlayerGameInfos.Sort([](const FPlayerGameCardInfo& A, const FPlayerGameCardInfo& B)
	{
		return CompareCards(A.CardsKeyInfo,B.CardsKeyInfo) == ECompareResult::Greater;
	});
	return PlayerGameInfos;
}

void AHoldemGameMode::StartNewGame()
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(UGameplayStatics::GetGameState(this));
	if(HoldemGameState->PlayerArray.Num() < 3)
	{
		UE_LOG(LogTemp,Error,TEXT("玩家数量小于3, 开始新游戏失败"));
		return;
	}
	HoldemGameState->OnNewGame();
	
	PlayerSidePot.Reset();
	ResetCardPool();
	
	UE_LOG(LogTemp,Log,TEXT("开始新游戏"));

	NotifyPlayerStartTurn();
	HoldemGameState->BlindsCount = 1;
}

void AHoldemGameMode::PrepareStartNewGame()
{
	// 给所有玩家发送开始游戏消息
	for(int32 i=0;i<GameState->PlayerArray.Num();i++)
	{
		AHoldemPlayerState* CurPlayerState = Cast<AHoldemPlayerState>(GameState->PlayerArray[i]);
		Cast<AHoldemPlayerController>(CurPlayerState->GetOwner())->OnNewGame();
	}
}

void AHoldemGameMode::SettleGame()
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);
	int32& RemainPot = HoldemGameState->Pot;
	TArray<AHoldemPlayerState*> SettlePlayers;
	const TArray<FPlayerGameStateInfo>& PlayerGameStateInfos = HoldemGameState->PlayerGameStateInfos;
	// 将未弃权的玩家进行结算
	for(const FPlayerGameStateInfo& PlayerGameStateInfo : PlayerGameStateInfos)
	{
		if(!PlayerGameStateInfo.bIsFold)
		{
			SettlePlayers.Add(PlayerGameStateInfo.Player);
		}
	}
	
	// 记录公共牌组的关键信息
	const FCardsKeyInfo PublicCardsKeyInfo = AnalyzeCardsKeyInfo(HoldemGameState->PublicCards);

	TArray<FPlayerGameCardInfo> PlayerGameInfos = AnalyzeAllPlayerCards(SettlePlayers);
	int32 index = 0;
	for(const FPlayerGameCardInfo& PlayerGameInfo : PlayerGameInfos)
	{
		// 首先判断最佳牌型是否和公共牌一样 ----> 剩余玩家平分底池
		if(PlayerGameInfo.CardsKeyInfo == PublicCardsKeyInfo)
		{
			TArray<AHoldemPlayerState*> EvenlySettlePlayers;
			for(int32 i = index;i<PlayerGameInfos.Num();i++)
			{
				EvenlySettlePlayers.Add(PlayerGameInfos[i].Player);
			}
			EvenlyDistributeChips(EvenlySettlePlayers,RemainPot);
			RemainPot = 0;
			UE_LOG(LogTemp, Warning, TEXT("剩余玩家最大牌和公共牌一样,平均分配剩余底池%d"),RemainPot);
			break;
		}

		// 找到玩家的游戏状态信息
		const FPlayerGameStateInfo PlayerGameStateInfo = *PlayerGameStateInfos.FindByPredicate([PlayerGameInfo](const FPlayerGameStateInfo& Item)
		{
			return Item.Player == PlayerGameInfo.Player;
		});

		// 判断玩家是否All-in
		if(PlayerGameStateInfo.bIsAllIn)
		{
			if(!PlayerSidePot.Contains(PlayerGameStateInfo.Player))
			{
				UE_LOG(LogTemp, Error, TEXT("玩家%s没有边池"),*PlayerGameStateInfo.Player->GetPlayerName());
				return;
			}
			// 给玩家分配经济奖励
			if(RemainPot > PlayerSidePot[PlayerGameStateInfo.Player])
			{
				PlayerGameStateInfo.Player->RemainChips += PlayerSidePot[PlayerGameStateInfo.Player];
				RemainPot -= PlayerSidePot[PlayerGameStateInfo.Player];
			}
			else
			{
				PlayerGameStateInfo.Player->RemainChips += RemainPot;
				RemainPot = 0;
			}
			
			// 底池已分配完毕
			if(RemainPot == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("玩家%s All-in 得到边池奖励%d 底池分配完毕"),*PlayerGameStateInfo.Player->GetPlayerName(),PlayerSidePot[PlayerGameStateInfo.Player]);
				break;
			}
			// 底池>0, 继续分配
			if(RemainPot > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("玩家%s All-in 得到边池奖励%d 底池剩余%d"),*PlayerGameStateInfo.Player->GetPlayerName(),PlayerSidePot[PlayerGameStateInfo.Player],RemainPot);
			}
		}
		// 没有All-in
		else
		{
			PlayerGameStateInfo.Player->RemainChips += RemainPot;
			UE_LOG(LogTemp, Warning, TEXT("玩家%s 得到底池奖励%d"),*PlayerGameStateInfo.Player->GetPlayerName(),RemainPot);
			RemainPot = 0;
			break;
		}
		
		index++;
	}

	// 分配奖池完毕
	if(RemainPot == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("结算游戏成功,分配奖池完毕"));
	}
	// 分配奖池未完毕
	else
	{
		UE_LOG(LogTemp, Error, TEXT("结算游戏有问题,分配奖池未完毕,剩余奖池:%d"),RemainPot);
	}
}

bool AHoldemGameMode::CanEnterNextRound()
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);

	// 处于盲注阶段特殊处理
	if(HoldemGameState->HoldemGameState == EHoldemGameState::Blinds)
	{
		if(HoldemGameState->BlindsCount  == 2)
		{
			return true;
		}
		HoldemGameState->BlindsCount++;
		return false;
	}
	
	TArray<FPlayerGameStateInfo> PlayerGameStateInfos = HoldemGameState->PlayerGameStateInfos;
	int32 PlayerChips = -1;
	for(const FPlayerGameStateInfo& PlayerGameStateInfo : PlayerGameStateInfos)
	{
		// 玩家是否弃牌/All-in
		if(PlayerGameStateInfo.bIsFold || PlayerGameStateInfo.bIsAllIn)
		{
			continue;
		}
		if(PlayerChips == -1)
		{
			PlayerChips = PlayerGameStateInfo.Chip;
		}
		else if(PlayerChips!= PlayerGameStateInfo.Chip)
		{
			return false;
		}
	}
	return true;
}

void AHoldemGameMode::TryEnterNextRound()
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);
	
	if(CanEnterNextRound())
	{
		// 玩家边池计算
		CalPlayersSidePot();
		
		if(HoldemGameState->HoldemGameState == EHoldemGameState::Blinds)
		{
			HoldemGameState->HoldemGameState = EHoldemGameState::PreFlop;
			// 给所有玩家发两张牌
			for(int32 i = 0;i<HoldemGameState->PlayerArray.Num();i++)
			{
				AHoldemPlayerState* PlayerState = Cast<AHoldemPlayerState>(HoldemGameState->PlayerArray[i]);
				PlayerState->AddCard(GetUnallocatedCard());
				PlayerState->AddCard(GetUnallocatedCard());
			}
		}
		else if(HoldemGameState->HoldemGameState == EHoldemGameState::PreFlop)
		{
			HoldemGameState->HoldemGameState = EHoldemGameState::Flop;
			// 添加3张公共牌
			for(int32 i = 0;i<3;i++)
			{
				HoldemGameState->AddPublicCard(GetUnallocatedCard());
			}
		}
		else if(HoldemGameState->HoldemGameState == EHoldemGameState::Flop)
		{
			HoldemGameState->HoldemGameState = EHoldemGameState::Turn;
			// 添加一张公共牌
			HoldemGameState->AddPublicCard(GetUnallocatedCard());
		}
		else if(HoldemGameState->HoldemGameState == EHoldemGameState::Turn)
		{
			HoldemGameState->HoldemGameState = EHoldemGameState::River;
			// 添加一张公共牌
			HoldemGameState->AddPublicCard(GetUnallocatedCard());
		}
		else if(HoldemGameState->HoldemGameState == EHoldemGameState::River)
		{
			HoldemGameState->HoldemGameState = EHoldemGameState::Showdown;
			// 结算游戏
			SettleGame();
			OnEndGame();
			return;
		}
		else if(HoldemGameState->HoldemGameState == EHoldemGameState::Showdown)
		{
			// 代码的结算阶段无意义
			return;
		}

		// 剩余可出牌玩家数量>1
		if (CountValidPlayers() > 1 || HoldemGameState->HoldemGameState == EHoldemGameState::PreFlop)
		{
			NotifyPlayerStartTurn();
		}
		else
		{
			// 判断牌是否发完
			int32 AddCardsNum = HoldemGameState->PublicCards.Num();
			for (int32 i = 0; i < 5 - AddCardsNum; i++)
			{
				HoldemGameState->AddPublicCard(GetUnallocatedCard());
			}
			// 结算游戏
			HoldemGameState->HoldemGameState = EHoldemGameState::Showdown;
			SettleGame();
			OnEndGame();
		}
	}
	else
	{
		NotifyPlayerStartTurn();
	}
}

void AHoldemGameMode::OnEndGame()
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);
	// 给所有玩家发送消息结束游戏
	for(int32 i = 0;i<HoldemGameState->PlayerArray.Num();i++)
	{
		AHoldemPlayerController* PlayerController = Cast<AHoldemPlayerController>(HoldemGameState->PlayerArray[i]->GetOwner());
		if(PlayerController)
		{
			PlayerController->OnEndGame();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("玩家%s没有PlayerController"),*HoldemGameState->PlayerArray[i]->GetPlayerName());
		}
	}
	K2_OnEndGame();
}

void AHoldemGameMode::OnPlayerEnsureTurn(const FPlayerTurnInfo& PlayerTurnInfo)
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);
	FPlayerGameStateInfo* PlayerGameStateInfo = HoldemGameState->PlayerGameStateInfos.FindByPredicate([PlayerTurnInfo](const FPlayerGameStateInfo& Item)
	{
		return Item.Player == PlayerTurnInfo.Player;
	});
	// 玩家弃牌
	if(PlayerTurnInfo.bIsFold)
	{
		PlayerGameStateInfo->bIsFold = true;
	}
	else
	{
		if(PlayerGameStateInfo->Player->RemainChips >= PlayerTurnInfo.Chips)
		{
			PlayerGameStateInfo->Player->RemainChips -= PlayerTurnInfo.Chips;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("玩家%s 筹码不足"),*PlayerGameStateInfo->Player->GetPlayerName());
			OnEndGame();
			return;
		}
		
		// 增加筹码
		PlayerGameStateInfo->Chip += PlayerTurnInfo.Chips;
		HoldemGameState->Pot += PlayerTurnInfo.Chips;
		
		// 玩家是否All-in
		if(PlayerTurnInfo.bIsAllIn)
		{
			PlayerGameStateInfo->bIsAllIn = true;
			TempPlayersSidePot.Add(PlayerGameStateInfo->Player,PlayerGameStateInfo->Chip);
		}
	}
	TryEnterNextRound();
}

bool AHoldemGameMode::NotifyPlayerStartTurn()
{
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);
	// 通知相关玩家开始 TODO:有bug, 未考虑玩家掉线问题
	HoldemGameState->CurrentPlayerIndex = (HoldemGameState->CurrentPlayerIndex + 1) % HoldemGameState->PlayerGameStateInfos.Num();
	for(const auto& PlayerGameStateInfo : HoldemGameState->PlayerGameStateInfos)
	{
		if(PlayerGameStateInfo.PlayerIndex == HoldemGameState->CurrentPlayerIndex)
		{
			if (PlayerGameStateInfo.bIsAllIn || PlayerGameStateInfo.bIsFold)
			{
				UE_LOG(LogTemp, Error, TEXT("序号%d的玩家弃牌或梭哈"),HoldemGameState->CurrentPlayerIndex);
				return NotifyPlayerStartTurn();
			}
			if(AHoldemPlayerController* PlayerController = Cast<AHoldemPlayerController>(PlayerGameStateInfo.Player->GetOwner()))
			{
				PlayerController->OnMyTurn();
				return true;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("没有找到序号%d的玩家,通知失败"),HoldemGameState->CurrentPlayerIndex);
	return false;
}

int32 AHoldemGameMode::CountValidPlayers() const
{
	// 找到所有未弃牌、All-in的玩家数量
	int32 ValidPlayerNum = 0;
	AHoldemGameStateBase* HoldemGameState = Cast<AHoldemGameStateBase>(GameState);
	for(const FPlayerGameStateInfo& PlayerGameStateInfo : HoldemGameState->PlayerGameStateInfos)
	{
		if(!PlayerGameStateInfo.bIsFold && !PlayerGameStateInfo.bIsAllIn)
		{
			ValidPlayerNum++;
		}
	}
	return ValidPlayerNum;
}
