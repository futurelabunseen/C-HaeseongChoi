// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterNonPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SuperSoldier.h"

ASSCharacterNonPlayer::ASSCharacterNonPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASSCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ASSCharacterNonPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASSCharacterNonPlayer::OnDead()
{
	Super::OnDead();

	ASSAIController* AIController = Cast<ASSAIController>(GetController());
	if (AIController)
	{
		AIController->StopAI();
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ASSCharacterNonPlayer::Dissolve, DissolveDelayTime, false);
}

void ASSCharacterNonPlayer::NetMulticastRpcShowAnimationMontage_Implementation(UAnimMontage* MontageToPlay, FName SectionName, const float AnimationSpeedRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(MontageToPlay, AnimationSpeedRate);

	if (SectionName != TEXT(""))
	{
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
	}
}

void ASSCharacterNonPlayer::Attack()
{
	const float AnimationSpeedRate = 1.5f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	NetMulticastRpcShowAnimationMontage(AttackMontage, TEXT(""), AnimationSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ASSCharacterNonPlayer::NotifyActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

const FHitResult ASSCharacterNonPlayer::AttackHitCheck()
{
	return FHitResult();
}

void ASSCharacterNonPlayer::NotifyActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	OnActionFinished.ExecuteIfBound();
}

void ASSCharacterNonPlayer::SetAIActionDelegate(const FAICharacterActionFinished& InOnActionFinished)
{
	OnActionFinished = InOnActionFinished;
}

void ASSCharacterNonPlayer::TurnInPlace(bool bTurnRight)
{
}

float ASSCharacterNonPlayer::GetAttackRange()
{
	return AttackRange;
}
