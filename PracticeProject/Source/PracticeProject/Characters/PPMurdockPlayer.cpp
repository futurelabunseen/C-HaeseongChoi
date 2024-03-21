// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PPMurdockPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APPMurdockPlayer::APPMurdockPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Set Skeletal Mesh and AnimInstanmce
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshRef(
		TEXT("/Game/ParagonMurdock/Characters/Heroes/Murdock/Meshes/Murdock.Murdock"));
	if (SkeletalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshRef.Object);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(
		TEXT("/Game/ParagonMurdock/Characters/Heroes/Murdock/Murdock_AnimBlueprint.Murdock_AnimBlueprint_C"));
	if (AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceRef.Class);
	}
}

void APPMurdockPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void APPMurdockPlayer::BeginPlay()
{
	Super::BeginPlay();

}

void APPMurdockPlayer::Move(const FInputActionValue& Value)
{
}

void APPMurdockPlayer::Look(const FInputActionValue& Value)
{
}
