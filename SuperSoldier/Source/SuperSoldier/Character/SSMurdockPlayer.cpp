// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSMurdockPlayer.h"

ASSMurdockPlayer::ASSMurdockPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Mesh & AnimInstance
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));

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

	// Fire Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Fire.AM_Fire"));
	if (FireMontageRef.Object)
	{
		FireMontage = FireMontageRef.Object;
	}

	// Throw Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ThrowMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Throw.AM_Throw"));
	if (ThrowMontageRef.Object)
	{
		ThrowMontage = ThrowMontageRef.Object;
	}

	// Call Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CallMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Call.AM_Call"));
	if (CallMontageRef.Object)
	{
		CallMontage = CallMontageRef.Object;
	}

	// Strata Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> StrataReadyMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_StrataReady.AM_StrataReady"));
	if (StrataReadyMontageRef.Object)
	{
		StrataReadyMontage = StrataReadyMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> StrataThrowMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_StrataThrow.AM_StrataThrow"));
	if (StrataThrowMontageRef.Object)
	{
		StrataThrowMontage = StrataThrowMontageRef.Object;
	}
}

void ASSMurdockPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Remove Gun
	//GetMesh()->HideBoneByName(TEXT("bot_hand"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("trap_handle"), EPhysBodyOp::PBO_None);
}
