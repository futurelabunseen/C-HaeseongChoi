// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Character/SSCharacterBase.h"

USSCharacterMovementComponent::USSCharacterMovementComponent()
{
	bSprint = false;
	WalkSpeed = 400.0f;
	SprintSpeed = 600.0f;
}

void USSCharacterMovementComponent::SetSprint(bool NewSprint)
{
	bSprint = NewSprint;
}

void USSCharacterMovementComponent::Sprint()
{
	if (CharacterOwner)
	{
		ASSCharacterBase* CharacterBase = Cast<ASSCharacterBase>(CharacterOwner);

		// 캐릭터 클래스가 유효한지 검사
		if (IsValid(CharacterBase))
		{
			if (CharacterBase->bDead)
			{
				return;
			}
		}

		// 달리기 상태에 맞게 변수를 설정
		if (bSprint)
		{
			MaxWalkSpeed = SprintSpeed;
			CharacterOwner->bUseControllerRotationYaw = false;
			bOrientRotationToMovement = true;
		}
		else
		{
			MaxWalkSpeed = WalkSpeed;
			CharacterOwner->bUseControllerRotationYaw = true;
			bOrientRotationToMovement = false;
		}
	}
}

void USSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	Sprint();
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

	Sprint();
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
}

uint8 FSSSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSprint)
	{
		Result |= FLAG_Custom_0;
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

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

// look through all of the variables that i want in my character movement
// set their respective saved variables
void FSSSavedMove_Character::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	USSCharacterMovementComponent* CharacterMovement = Cast<USSCharacterMovementComponent>(C->GetCharacterMovement());
	bSprint = CharacterMovement->bSprint;
}

// Take the data in the saved move
// apply to the current state of the character movementop
void FSSSavedMove_Character::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	USSCharacterMovementComponent* CharacterMovement = Cast<USSCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bSprint = bSprint;
}
