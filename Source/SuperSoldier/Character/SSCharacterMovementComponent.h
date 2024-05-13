// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SSCharacterMovementComponent.generated.h"

// Movement Component's Saved Data
// Replicating minimal version of it to the server
class FSSSavedMove_Character : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;
public:
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;


	uint8 bSprint : 1;
	uint8 bAiming : 1;
};

// Indicating using custom
class FSSNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;
public:
	FSSNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	USSCharacterMovementComponent();

	void SetSprint(bool NewSprint);
	void SetAiming(bool NewAiming);
protected:
	virtual void Sprint();
	virtual void Aiming();
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
public:
	// Whether Client's Sprint
	// Will be replicated to server
	uint8 bSprint : 1;
	uint8 bAiming : 1;
protected:
	float WalkSpeed;
	float SprintSpeed;
};
