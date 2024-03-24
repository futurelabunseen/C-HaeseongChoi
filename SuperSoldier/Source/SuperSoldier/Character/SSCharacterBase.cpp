// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_SSCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
		TEXT("/Game/ParagonMurdock/Characters/Heroes/Murdock/Skins/A_Executioner/Mesh/Murdock_Executioner.Murdock_Executioner"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	} 

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/ABP_MurdockPlayer.ABP_MurdockPlayer_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<USSCharacterControlData> NormalModeRef(
		TEXT("/Game/SuperSoldier/Characters/CharacterControl/DA_NormalMode.DA_NormalMode"));
	if (NormalModeRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Normal, NormalModeRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<USSCharacterControlData> AimModeRef(
		TEXT("/Game/SuperSoldier/Characters/CharacterControl/DA_AimMode.DA_AimMode"));
	if (AimModeRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Aiming, AimModeRef.Object);
	}
}

void ASSCharacterBase::SetCharacterControlData(const USSCharacterControlData* CharacterControlData)
{
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;
	
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->MaxWalkSpeed = CharacterControlData->MaxWalkSpeed;
}

void ASSCharacterBase::AttackHitCheck()
{
	UE_LOG(LogTemp, Log, TEXT("ASSCharacterBase::AttackHitCheck"));
}


