// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_RespawnTankPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"

ASS_RespawnTankPlayer::ASS_RespawnTankPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Pawn
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}

	// Capsule
	{
		GetCapsuleComponent()->InitCapsuleSize(100.0f, 100.0f);
	}

	// Movement
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
		GetCharacterMovement()->AirControl = 10.0f;
	}

	// Camera
	{
		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(RootComponent);
		FollowCamera->SetRelativeLocation(FVector(150.0f, 0.0f, 800.0f));
		FollowCamera->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f).Quaternion());
		FollowCamera->bUsePawnControlRotation = false;
	}

	// Mesh
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
			TEXT("/Game/SuperSoldier/Characters/RespawnTank/SK_RespawnTank.SK_RespawnTank"));
		if (CharacterMeshRef.Object)
		{
			GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
		}

		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -300.0f), FRotator(0.0f, 0.0f, 0.0f));
		GetMesh()->SetRelativeScale3D(FVector(0.5f));
	}
}

void ASS_RespawnTankPlayer::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		if (PlayerController)
		{
			DisableInput(PlayerController);
		}
	}
}

void ASS_RespawnTankPlayer::BeginPlay()
{
	Super::BeginPlay();

	// If Locally Controlled
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		if (PlayerController)
		{
			EnableInput(PlayerController);
		}
	}
}

void ASS_RespawnTankPlayer::SetRespawnMurdockCharacter(ACharacter* NewRespawnMurdockCharacter)
{
	check(NewRespawnMurdockCharacter)

	RespawnMurdockCharacter = NewRespawnMurdockCharacter;

	FName SocketName = TEXT("Socket_MurdockCharacter");
	NewRespawnMurdockCharacter->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName);
}
