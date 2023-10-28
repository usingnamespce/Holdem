// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldemPlayerState.h"
#include "HoldemCardBag.h"
#include "Net/UnrealNetwork.h"

void AHoldemPlayerState::AddCard(FCardInfo NewCardInfo)
{
	if(HasAuthority())
	{
		PlayerCards.Add(NewCardInfo);
		OnRep_PlayerCards();
	}
}

TArray<FCardInfo> AHoldemPlayerState::GetCardsInfo()
{
	return PlayerCards;
}

void AHoldemPlayerState::SetPlayerLocationInfo(FPlayerLocationInfo InPlayerLocationInfo)
{
	if(HasAuthority())
	{
		PlayerLocationInfo = InPlayerLocationInfo;
		// 生成卡牌背包
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		CardBag = GetWorld()->SpawnActor<AHoldemCardBag>(CardBagClass,FTransform(PlayerLocationInfo.CardLocation), SpawnParameters);
	}
}

void AHoldemPlayerState::BeginPlay()
{
	Super::BeginPlay();


}

void AHoldemPlayerState::OnRep_PlayerCards()
{
	if(OnPlayerCardChange.IsBound())
	{
		OnPlayerCardChange.Broadcast();
	}
}

void AHoldemPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemPlayerState,PlayerCards,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemPlayerState,PlayerLocationInfo,COND_None,REPNOTIFY_Always);
}
