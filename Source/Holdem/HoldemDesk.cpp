

#include "HoldemDesk.h"

#include "HoldemGameMode.h"

AHoldemDesk::AHoldemDesk()
{
	
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AHoldemDesk::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		Cast<AHoldemGameMode>(GetWorld()->GetAuthGameMode())->HoldemDesk = this;
	}
}



