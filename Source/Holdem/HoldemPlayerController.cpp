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
	UE_LOG(LogTemp,Warning,TEXT("玩家%s收到消息：开始自己回合"),*PlayerState->GetPlayerName());
}

void AHoldemPlayerController::EnsureMyTurn_Implementation(FPlayerTurnInfo PlayerTurnInfo)
{
	if(!IsValid(PlayerTurnInfo.Player))
	{
		UE_LOG(LogTemp,Error,TEXT("EnsureMyTurn_Implementation: PlayerTurnInfo.Player is invalid!"));
		return;
	}
	UE_LOG(LogTemp,Warning,TEXT("服务器收到玩家%s消息：确认自己回合"),*PlayerState->GetPlayerName());
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

void AHoldemPlayerController::PlayerReady_Implementation(int32 PlayerRemainChips)
{
	if(AHoldemGameMode* GameMode = Cast<AHoldemGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnPlayerReady(PlayerRemainChips,Cast<AHoldemPlayerState>(PlayerState));
		UE_LOG(LogTemp,Warning,TEXT("玩家%s收到消息：准备游戏"),*PlayerState->GetPlayerName());
	}
}
