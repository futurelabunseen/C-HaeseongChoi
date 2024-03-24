// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_SSCAPSULE);

	// Mesh
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void ASSCharacterBase::AttackHitCheck()
{
}


