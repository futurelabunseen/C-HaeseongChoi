// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SSCharacterControlData.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	USSCharacterControlData();

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint32 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FVector RelativeLocation;

	UPROPERTY(EditAnywhere, Category = Widget)
	uint32 bCrosshairVisibility : 1;
};
