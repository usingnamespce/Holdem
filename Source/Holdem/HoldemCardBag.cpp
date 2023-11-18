
#include "HoldemCardBag.h"

#include "HoldemPlayerState.h"
#include "Kismet/GameplayStatics.h"

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
	/*if(HasAuthority() && CardClass)
	{
		if(CardPool.Num() >= OwnerPlayerState->GetCardsInfo().Num())
		{
			return;
		}
		FTransform SpawnTransform = FTransform();
		SpawnTransform.SetLocation(SpawnLocationOffset*CardPool.Num() + GetActorLocation());
		SpawnTransform.SetScale3D(FVector(0.3));
		AHoldemCard* SpawnCard =  GetWorld()->SpawnActorDeferred<AHoldemCard>(CardClass,SpawnTransform);
		if(SpawnCard)
		{
			SpawnCard->SetCardInfo(OwnerPlayerState->GetCardsInfo()[OwnerPlayerState->GetCardsInfo().Num()-1]);
			SpawnCard->SetOwner(GetOwner());
			UGameplayStatics::FinishSpawningActor(SpawnCard,SpawnTransform);
		}
		CardPool.Add(SpawnCard);
	}
	K2_OnOwnerCardChange();*/
}


