// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_Scorobey.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSScorobeyAIController.h"

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
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// Mesh & AnimInstance
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -75.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	// AI
	AIControllerClass = ASSScorobeyAIController::StaticClass();
}
