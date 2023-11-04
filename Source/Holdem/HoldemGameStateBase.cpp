// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldemGameStateBase.h"

#include "HoldemCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

void AHoldemGameStateBase::OnRep_PublicCards()
{
	if(AHoldemCharacter* Character = Cast<AHoldemCharacter>(UGameplayStatics::GetPlayerController(this,0)->GetPawn()))
	{
		Character->HandleAddPublicCard(PublicCards);
	}
}

void AHoldemGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemGameStateBase,PublicCards,COND_None,REPNOTIFY_Always);
}
