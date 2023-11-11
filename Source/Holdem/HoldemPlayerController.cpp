// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldemPlayerController.h"

#include "HoldemGameMode.h"
#include "Kismet/GameplayStatics.h"

void AHoldemPlayerController::OnMyTurn_Implementation()
{
	if(OnMyTurnDelegate.IsBound())
	{
		OnMyTurnDelegate.Broadcast();
	}
}

void AHoldemPlayerController::EnsureMyTurn_Implementation(FPlayerTurnInfo PlayerTurnInfo)
{
	if(AHoldemGameMode* GameMode = Cast<AHoldemGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnPlayerEnsureTurn(PlayerTurnInfo);
	}
}

void AHoldemPlayerController::OnNewGame_Implementation()
{
	if(OnNewGameDelegate.IsBound())
	{
		OnNewGameDelegate.Broadcast();
	}
}

void AHoldemPlayerController::OnEndGame_Implementation()
{
	if(OnEndGameDelegate.IsBound())
	{
		OnEndGameDelegate.Broadcast();
	}
}
