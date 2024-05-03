// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	USSAnimInstance();
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetPitchOffset();
	void SetYawOffset();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsDead : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreashould;

// TurnInPlace
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float Direction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float Pitch = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float YawActual = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float Yaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float RootYawOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float DistanceCurveValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float MaxTurnAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FName DistanceCurve = TEXT("DistanceCurve");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FName TurnInPlace = TEXT("Turn");

	float YawLastTick = 0.f;
	float YawChangeOverFrame = 0.f;
	float DistanceCurveValueLastFrame = 0.f;
	float TurnDirection = 0.f;
	float ABSRootYawOffset = 0.f;
	float DistanceCurveDifference = 0.f;
	float YawToSubtract = 0.f;
	float YawMultiplier = 0.f;
};
