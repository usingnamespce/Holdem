
#include "HoldemCardBag.h"

#include "HoldemPlayerState.h"

AHoldemCardBag::AHoldemCardBag()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AHoldemCardBag::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayerState = Cast<AHoldemPlayerState>(GetOwner());
	if(OwnerPlayerState)
	{
		OwnerPlayerState->OnPlayerCardChange.AddDynamic(this, &AHoldemCardBag::OnOwnerCardChange);
	}
}

void AHoldemCardBag::OnOwnerCardChange()
{
	K2_OnOwnerCardChange();
}


