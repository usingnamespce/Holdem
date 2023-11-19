// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HoldemCard.h"
#include "HoldemCharacter.generated.h"

UCLASS(config=Game)
class AHoldemCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AHoldemCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	UFUNCTION()
	void OnCardsChange();
public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintImplementableEvent)
	void HandleAddPublicCard(const TArray<FCardInfo>& PublicCards);
	UFUNCTION(BlueprintImplementableEvent)
	void HandleAddPrivateCard(const TArray<FCardInfo>& PrivateCards);
	// 主控端才调用
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPlayerStateSet();
	UFUNCTION(BlueprintPure)
	bool HoldemIsLocalController() const;
};

