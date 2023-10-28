

#include "HoldemDesk.h"

#include "HoldemGameMode.h"

AHoldemDesk::AHoldemDesk()
{
	
	PrimaryActorTick.bCanEverTick = false;

	DeskComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeskComponent"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DeskComponent->SetupAttachment(RootComponent);
}

void AHoldemDesk::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		Cast<AHoldemGameMode>(GetWorld()->GetAuthGameMode())->HoldemDesk = this;
	}
}



