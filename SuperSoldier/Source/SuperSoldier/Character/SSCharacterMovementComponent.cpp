// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterMovementComponent.h"
#include "GameFramework/Character.h"

USSCharacterMovementComponent::USSCharacterMovementComponent()
{
	bSprint = false;
	bAiming = false;
	WalkSpeed = 400.0f;
	SprintSpeed = 600.0f;
}

void USSCharacterMovementComponent::SetSprint(bool NewSprint)
{
	bSprint = NewSprint;
}

void USSCharacterMovementComponent::SetAiming(bool NewAiming)
{
	bAiming = NewAiming;
}

void USSCharacterMovementComponent::Sprint()
{
	if (CharacterOwner)
	{
		if (bSprint)
		{
			MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			MaxWalkSpeed = WalkSpeed;
		}
	}
}

void USSCharacterMovementComponent::Aiming()
{
	if (CharacterOwner)
	{
		if (bAiming)
		{
			CharacterOwner->bUseControllerRotationYaw = true;
			bOrientRotationToMovement = false;
		}
		else
		{
			CharacterOwner->bUseControllerRotationYaw = false;
			bOrientRotationToMovement = true;
		}
	}
}

void USSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	
	/*if (FMath::IsNearlyZero(Velocity.Size2D()))
		CharacterOwner->bUseControllerRotationYaw = true;
	else
		CharacterOwner->bUseControllerRotationYaw = false;*/

	Sprint();
	Aiming();
}

FNetworkPredictionData_Client* USSCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		USSCharacterMovementComponent* MutableThis = const_cast<USSCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FSSNetworkPredictionData_Client_Character(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

// Set the state of the character movement component
void USSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bAiming = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	Sprint();
	Aiming();
}

FSSNetworkPredictionData_Client_Character::FSSNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FSSNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FSSSavedMove_Character());
}

void FSSSavedMove_Character::Clear()
{
	Super::Clear();
	bSprint = false;
	bAiming = false;
}

uint8 FSSSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSprint)
	{
		Result |= FLAG_Custom_0;
	}

	if (bAiming)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

// Check current move and new move
// Check Can we combine these two moves
// For saving bandwith
// if return is true don't need to send
bool FSSSavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSSSavedMove_Character* NewCharacterMove = static_cast<FSSSavedMove_Character*>(NewMove.Get());
	if (bSprint != NewCharacterMove->bSprint)
	{
		return false;
	}

	if (bAiming != NewCharacterMove->bAiming)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

// look through all of the variables that i want in my character movement
// set their respective saved variables
void FSSSavedMove_Character::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	USSCharacterMovementComponent* CharacterMovement = Cast<USSCharacterMovementComponent>(C->GetCharacterMovement());
	bSprint = CharacterMovement->bSprint;
	bAiming = CharacterMovement->bAiming;
}

// Take the data in the saved move
// apply to the current state of the character movementop
void FSSSavedMove_Character::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	USSCharacterMovementComponent* CharacterMovement = Cast<USSCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bSprint = bSprint;
	CharacterMovement->bAiming = bAiming;
}
