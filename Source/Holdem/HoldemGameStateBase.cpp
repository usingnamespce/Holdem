// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldemGameStateBase.h"

#include "HoldemCharacter.h"
#include "HoldemGameMode.h"
#include "HoldemPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AHoldemGameStateBase::AHoldemGameStateBase()
{
	CurrentPlayerIndex = -1;
	Pot = 0;
	HoldemGameState = EHoldemGameState::Blinds;
}

void AHoldemGameStateBase::AddPublicCard(FCardInfo NewCard)
{
	if(HasAuthority())
	{
		PublicCards.Add(NewCard);
		OnRep_PublicCards();
		UE_LOG(LogTemp,Log,TEXT("添加公共牌, 现有数量:%d"),PublicCards.Num());
	}
}

void AHoldemGameStateBase::ResetPublicCardsPool()
{
	PublicCards.Reset();
	OnRep_PublicCards();
}

void AHoldemGameStateBase::OnNewGame()
{
	ResetPublicCardsPool();
	HoldemGameState = EHoldemGameState::Blinds;
	PlayerGameStateInfos.Reset();
	Pot = 0;
	for(APlayerState* CurPlayerState : PlayerArray)
	{
		FPlayerGameStateInfo TempPlayerGameStateInfo;
		TempPlayerGameStateInfo.Player = Cast<AHoldemPlayerState>(CurPlayerState);
		TempPlayerGameStateInfo.Player->ClearPlayerCards();
		TempPlayerGameStateInfo.Chip = 0;
		TempPlayerGameStateInfo.PlayerIndex = TempPlayerGameStateInfo.Player->PlayerIndex;
		TempPlayerGameStateInfo.bIsFold = false;
		
		PlayerGameStateInfos.Add(TempPlayerGameStateInfo);
	}
}

int32 AHoldemGameStateBase::GetMaxChips(AHoldemPlayerState* Player) const
{
	int32 MaxChips = 0;
	int32 PlayerChips = 0;
	for(const FPlayerGameStateInfo& CurPlayerGameStateInfo : PlayerGameStateInfos)
	{
		if(CurPlayerGameStateInfo.Chip > MaxChips)
		{
			MaxChips = CurPlayerGameStateInfo.Chip;
		}
		if(CurPlayerGameStateInfo.Player == Player)
		{
			PlayerChips = CurPlayerGameStateInfo.Chip;
		}
	}
	return MaxChips - PlayerChips;
}

void AHoldemGameStateBase::OnRep_PublicCards()
{
	if(AHoldemCharacter* Character = Cast<AHoldemCharacter>(UGameplayStatics::GetPlayerController(this,0)->GetPawn()))
	{
		Character->HandleAddPublicCard(PublicCards);
	}
}

void AHoldemGameStateBase::OnRep_PlayerGameStateInfos()
{
	
}

void AHoldemGameStateBase::OnRep_HoldemGameState()
{
	
}

void AHoldemGameStateBase::OnRep_Pot()
{
	
}

void AHoldemGameStateBase::OnRep_CurrentPlayerIndex()
{
	
}

void AHoldemGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,PublicCards,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,PlayerGameStateInfos,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,HoldemGameState,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,Pot,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,CurrentPlayerIndex,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,BlindsCount,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,BlindMinChips,COND_None,REPNOTIFY_Always);
}
