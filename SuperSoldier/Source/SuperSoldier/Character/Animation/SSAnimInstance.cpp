// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/SSAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

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

void USSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ASSCharacterBase* OwningCharacter = Cast<ASSCharacterBase>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		bIsDead = OwningCharacter->bDead;
	}

	if (Movement)
	{
		Velocity = Movement->Velocity;
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreashould;
	}

	SetPitchOffset();
	SetYawOffset();

	YawActual = RootYawOffset * -1.0f;
}

void USSAnimInstance::SetPitchOffset()
{
	if (Owner)
	{
		FRotator PawnRotation = Owner->GetActorRotation();
		FRotator AimRotation = Owner->GetBaseAimRotation();
		Pitch = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, PawnRotation).Pitch;
	}
}

void USSAnimInstance::SetYawOffset()
{
	if (GroundSpeed > 0.f || IsAnyMontagePlaying())
	{
		RootYawOffset = 0.0f;
		return;
	}

	if (Owner)
	{
		YawLastTick = Yaw;
		Yaw = Owner->GetActorRotation().Yaw;
		YawChangeOverFrame = YawLastTick - Yaw;

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset + YawChangeOverFrame);

		if (GetCurveValue(TurnInPlace) > 0.f)
		{
			DistanceCurveValueLastFrame = DistanceCurveValue;
			DistanceCurveValue = GetCurveValue(DistanceCurve);

			(RootYawOffset > 0.f) ? TurnDirection = -1.f : TurnDirection = 1.f;
			DistanceCurveDifference = DistanceCurveValueLastFrame - DistanceCurveValue;

			RootYawOffset = RootYawOffset - (DistanceCurveDifference * TurnDirection);
			ABSRootYawOffset = UKismetMathLibrary::Abs(RootYawOffset);
			if (ABSRootYawOffset > MaxTurnAngle)
			{
				YawToSubtract = ABSRootYawOffset - MaxTurnAngle;
				YawMultiplier = 1.f;
				(RootYawOffset > 0.f) ? YawMultiplier = 1.f : YawMultiplier = -1.f;

				RootYawOffset = RootYawOffset - YawToSubtract * YawMultiplier;
			}
		}
	}
}
