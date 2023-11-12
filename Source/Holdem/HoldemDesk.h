// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoldemDesk.generated.h"

class UStaticMeshComponent;

// 玩家初始的位置消息
USTRUCT(BlueprintType)
struct FPlayerLocationInfo
{
	GENERATED_BODY()
	
	// 出生位置
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FTransform StartTransform;

	// 卡牌背包位置
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector CardLocation;
};

UCLASS()
class HOLDEM_API AHoldemDesk : public AActor
{
	GENERATED_BODY()
	
public:	
	AHoldemDesk();
	
private:
	
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<FPlayerLocationInfo> PlayersLocationInfos;

private:
	
};
