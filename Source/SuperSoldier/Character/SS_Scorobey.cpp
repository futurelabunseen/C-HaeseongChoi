// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_Scorobey.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSScorobeyAIController.h"
#include "Physics/SSColision.h"
#include "Engine/DamageEvents.h"
#include "Character/SSCharacterPlayer.h"

ASS_Scorobey::ASS_Scorobey(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Scorobey/Meshes/SK_Scorobey.SK_Scorobey"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Scorobey/Animations/ABP_Scorobey.ABP_Scorobey_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Scorobey/Animations/AM_ScorobeyAttack.AM_ScorobeyAttack"));
	if (AttackMontageRef.Object)
	{
		AttackMontage = AttackMontageRef.Object;
	}

	bTurnInPlace = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(75.f, 75.0f);

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = 250.0f;

	// Mesh & AnimInstance
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -75.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	// AI
	AttackRange = 140.0f;
	AIControllerClass = ASSScorobeyAIController::StaticClass();

	// SFX
	static ConstructorHelpers::FObjectFinder<USoundBase> AttackSoundRef(TEXT("/Game/Small_Monster_SFX_1/Cues/Mala_Attack_01_Cue.Mala_Attack_01_Cue"));
	if (AttackSoundRef.Object)
	{
		AttackSound = AttackSoundRef.Object;
	}
}

void ASS_Scorobey::AttackHitCheck()
{
	Super::AttackHitCheck();

	if (HasAuthority())
	{
		FVector CollisionCenter = GetActorLocation() + GetActorForwardVector() * 80.0f;
		float CollisionRadius = 50.0f;

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		TArray<FOverlapResult> OverlapResults;
		FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(CollisionRadius);

		bool bHitDetected = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			CollisionCenter,
			FQuat::Identity,
			CCHANNEL_SSACTION,
			CollisionSphere,
			CollisionParams);


#if ENABLE_DRAW_DEBUG
		FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), CollisionCenter, CollisionRadius, 18, DrawColor, false, 2.0f);
#endif
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* OverlapActor = OverlapResult.GetActor();
			ASSCharacterPlayer* CharacterPlayer = Cast<ASSCharacterPlayer>(OverlapActor);

			if (CharacterPlayer)
			{
				FDamageEvent DamageEvent;
				const float AttackDamage = 30.0f;
				CharacterPlayer->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
			}
		}
	}
}
