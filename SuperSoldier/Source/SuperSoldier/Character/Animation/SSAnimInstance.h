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
	void SetAimRotation(float DeltaSeconds);

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float AimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float AimYaw;
};
