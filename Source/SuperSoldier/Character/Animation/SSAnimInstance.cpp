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
	bIsFullBody = GetCurveValue(FullBody) > 0.f;
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
	if (Owner)
	{
		YawLastTick = Yaw;
		Yaw = Owner->GetActorRotation().Yaw;

		if (GroundSpeed > 0.f || GetCurveValue(ResetRootYaw) > 0.f)
		{
			RootYawOffset = 0.0f;
			return;
		}

		YawChangeOverFrame = YawLastTick - Yaw;
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset + YawChangeOverFrame);

		ProcessTurnInPlace();
	}
}

void USSAnimInstance::ProcessTurnInPlace()
{
	// TurnInPlace 애니메이션이 재생 중이라면
	if (GetCurveValue(TurnInPlace) > 0.f)
	{
		// 회전 방향을 결정
		(RootYawOffset > 0.f) ? TurnDirection = -1.f : TurnDirection = 1.f;

		// TurnInPlace를 위한 회전 값 Delta를 계산
		DistanceCurveValueLastFrame = DistanceCurveValue;
		DistanceCurveValue = GetCurveValue(DistanceCurve);
		DistanceCurveDifference = DistanceCurveValueLastFrame - DistanceCurveValue;

		// Root를 회전시키던 값을 0.0f로 서서히 되돌린다.
		RootYawOffset = RootYawOffset - (DistanceCurveDifference * TurnDirection);

		// 예상치 못한 회전을 방지하기 위해 최대 회전 각도를 벗어나려고 하면 값을 변경해준다.
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
