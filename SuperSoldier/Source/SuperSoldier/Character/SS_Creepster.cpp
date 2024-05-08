// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_Creepster.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSCreepsterAIController.h"

ASS_Creepster::ASS_Creepster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Creepster/Mesh/SK_Creepster.SK_Creepster"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Creepster/Animations/ABP_Creepster.ABP_Creepster_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Creepster/Animations/AM_CreepsterAttack.AM_CreepsterAttack"));
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
	AIControllerClass = ASSCreepsterAIController::StaticClass();
}

