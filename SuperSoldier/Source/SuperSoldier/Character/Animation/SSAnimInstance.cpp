// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/SSAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USSAnimInstance::USSAnimInstance()
{
	MovingThreashould = 3.0f;
}

void USSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void USSAnimInstance::SetAimRotation(float DeltaSeconds)
{
	FRotator ControlRotation = Owner->GetControlRotation();
	FRotator ActorRotation = Owner->GetActorRotation();
	FRotator DeltaRotation = ControlRotation - ActorRotation;
	DeltaRotation.Normalize();

	FRotator CurRotator = FRotator(AimPitch, AimYaw, 0.0f);

	FRotator ResultRotator = FMath::RInterpTo(CurRotator, DeltaRotation, DeltaSeconds, 15.0f);

	AimPitch = ResultRotator.Pitch;
	AimYaw = ResultRotator.Yaw;

	// AimPitch = FMath::ClampAngle(ResultRotator.Pitch, -90.0f, 90.0f);
	// AimYaw = FMath::ClampAngle(ResultRotator.Yaw, -90.0f, 90.0f);
}

void USSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreashould;
		SetAimRotation(DeltaSeconds);
	}
}
