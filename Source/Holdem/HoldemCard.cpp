
#include "HoldemCard.h"

#include "Net/UnrealNetwork.h"


AHoldemCard::AHoldemCard()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

}

void AHoldemCard::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		SetCardInfo(CardInfo);
	}
}

void AHoldemCard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AHoldemCard,CardInfo,COND_OwnerOnly,REPNOTIFY_Always);
}

void AHoldemCard::OnRep_CardInfo()
{
	SetCardInfo(CardInfo);
}

FCardInfo AHoldemCard::CardIdToCardInfo(int32 CardID)
{
	FCardInfo NewCardInfo;
	
	NewCardInfo.CardID = CardID;
	
	int32 CardType = CardID/13;
	NewCardInfo.CardNumber = CardID%13 + 2;
	
	if(CardType == 0)
	{
		NewCardInfo.Type = ECardType::Club;
	}
	else if(CardType == 1)
	{
		NewCardInfo.Type = ECardType::Heart;
	}
	else if(CardType == 2)
	{
		NewCardInfo.Type = ECardType::Dianmond;
	}
	else
	{
		NewCardInfo.Type = ECardType::Spade;
	}
	
	return NewCardInfo;
}


