// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterNonPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SuperSoldier.h"
#include "Kismet/GameplayStatics.h"

ASSCharacterNonPlayer::ASSCharacterNonPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultWalkSpeed = 0.0f;

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

void ASSCharacterNonPlayer::SetDead()
{
	Super::SetDead();
	
	SetLifeSpan(15.0f);

	ASSAIController* AIController = Cast<ASSAIController>(GetController());
	if (AIController)
	{
		AIController->StopAI();
	}
}

void ASSCharacterNonPlayer::OnRep_ServerCharacterbDead()
{
	Super::OnRep_ServerCharacterbDead();

	if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{
		FTimerHandle DeadTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ASSCharacterNonPlayer::Dissolve, DissolveDelayTime, false);
		PlayDeadSound();
	}
}

float ASSCharacterNonPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (GetCharacterMovement())
	{
		FTimerHandle SpeedRestoreTimerHandle;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SpeedReductionFactor;
		GetWorldTimerManager().SetTimer(SpeedRestoreTimerHandle, FTimerDelegate::CreateLambda([&] {
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		}), 1.25f, false);
	}

	return Result;
}

void ASSCharacterNonPlayer::Attack(FName AttackMontageSectionName)
{
	const float AnimationSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	NetMulticastRpcShowAnimationMontageWithSection(AttackMontage, AttackMontageSectionName, AnimationSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ASSCharacterNonPlayer::NotifyActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void ASSCharacterNonPlayer::AttackHitCheck(FName AttackId)
{
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

void ASSCharacterNonPlayer::PlaySoundEffect()
{
	if (AttackSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			AttackSound,
			GetActorLocation());
	}
}

void ASSCharacterNonPlayer::PlayMoanSound()
{
	Super::PlayMoanSound();

	UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(),
		MoanSound,
		GetActorLocation());
}

void ASSCharacterNonPlayer::PlayDeadSound()
{
	Super::PlayDeadSound();

	UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(),
		DeadSound,
		GetActorLocation(),
		FRotator::ZeroRotator,
		0.7f);
}
