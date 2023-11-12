// Copyright Epic Games, Inc. All Rights Reserved.

#include "HoldemCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HoldemGameStateBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


AHoldemCharacter::AHoldemCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; 	
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
}


void AHoldemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHoldemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHoldemCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHoldemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHoldemCharacter::LookUpAtRate);
	
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AHoldemCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AHoldemCharacter::TouchStopped);
	
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AHoldemCharacter::OnResetVR);
}

void AHoldemCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority() && HoldemIsLocalController())
	{
		AHoldemPlayerState* HoldemPlayerState = Cast<AHoldemPlayerState>(GetPlayerState());
		if (HoldemPlayerState)
		{
			HoldemPlayerState->OnPlayerCardChange.AddDynamic(this, &AHoldemCharacter::OnCardsChange);
		}
		K2_OnPlayerStateSet();
	}
}

void AHoldemCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if(HoldemIsLocalController())
	{
		AHoldemPlayerState* HoldemPlayerState = Cast<AHoldemPlayerState>(GetPlayerState());
		if (HoldemPlayerState)
		{
			HoldemPlayerState->OnPlayerCardChange.AddDynamic(this, &AHoldemCharacter::OnCardsChange);
		}
		K2_OnPlayerStateSet();
	}
}

void AHoldemCharacter::OnCardsChange()
{
	AHoldemPlayerState* HoldemPlayerState = Cast<AHoldemPlayerState>(GetPlayerState());
	HandleAddPrivateCard(HoldemPlayerState->GetCardsInfo());
}

bool AHoldemCharacter::HoldemIsLocalController() const
{
	return GetController() == UGameplayStatics::GetPlayerController(this,0);
}

void AHoldemCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AHoldemCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AHoldemCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AHoldemCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHoldemCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHoldemCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHoldemCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}
