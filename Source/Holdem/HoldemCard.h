// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoldemCard.generated.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	//梅花
	Club		UMETA(DisplayName = "梅花"),
	//方块
	Dianmond	UMETA(DisplayName = "方块"),
	//红桃
	Heart		UMETA(DisplayName = "红桃"),
	//黑桃
	Spade		UMETA(DisplayName = "黑桃")
};

USTRUCT(BlueprintType)
struct FCardInfo
{
	GENERATED_BODY()

	//牌的花色
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	ECardType Type;

	//牌的大小
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 CardNumber;
	
	//牌的ID
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 CardID;
};

UCLASS()
class HOLDEM_API AHoldemCard : public AActor
{
	GENERATED_BODY()
	
public:
	
	AHoldemCard();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetCardID(int32 InCardID);
	
private:
	
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FCardInfo CardInfo;
};
