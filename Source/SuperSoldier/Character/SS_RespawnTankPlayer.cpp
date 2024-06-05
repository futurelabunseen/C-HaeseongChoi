// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_RespawnTankPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Character/SS_MurdockPlayer.h"
#include "SuperSoldier.h"

ASS_RespawnTankPlayer::ASS_RespawnTankPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Pawn
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}

	// Capsule
	{
		GetCapsuleComponent()->InitCapsuleSize(100.0f, 100.0f);
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("SSRespawnTankCapsule"));
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

	GetCharacterMovement()->SetMovementMode(MOVE_None);

	if (HasAuthority())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ASSCharacterPlayer* PlayerCharacter = Cast<ASSCharacterPlayer>(MurdockCharacter);
		check(PlayerCharacter);
		PlayerCharacter->SetActorHiddenInGame(false);
		PlayerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASS_RespawnTankPlayer::RespawnMurdockCharacter, 0.016f, false);
	}

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

	SetActorTickEnabled(false);

	if (HasAuthority())
	{
		MurdockCharacter = GetWorld()->SpawnActor<ASS_MurdockPlayer>(ASS_MurdockPlayer::StaticClass());

		FName SocketName = TEXT("Socket_MurdockCharacter");
		MurdockCharacter->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName);

		MurdockCharacter->SetActorHiddenInGame(true);
		MurdockCharacter->SetCharacterCollisionType(ECharacterCollisionType::NoCollision);
	}
	else
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

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

void ASS_RespawnTankPlayer::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (Controller)
	{
		Controller->SetControlRotation(FRotator::ZeroRotator);
	}
}

void ASS_RespawnTankPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASS_RespawnTankPlayer::RespawnMurdockCharacter()
{
	check(MurdockCharacter);

	FName RespawnStartLocationSocketName = TEXT("Socket_MurdockCharacter");
	FName RespawnEndLocationSocketName = TEXT("Socket_RespawnComplete");

	FVector RespawnStartLocation = GetMesh()->GetSocketTransform(RespawnStartLocationSocketName).GetLocation();
	FVector RespawnEndLocation = GetMesh()->GetSocketTransform(RespawnEndLocationSocketName).GetLocation();
	FVector LocationDist = RespawnEndLocation - RespawnStartLocation;
	RespawnEndLocation += LocationDist * 0.5f;

	FVector MurdockCurLocation = MurdockCharacter->GetActorLocation();

	FVector FinalLocation = FMath::Lerp(MurdockCurLocation, RespawnEndLocation, 0.1f);
	MurdockCharacter->SetActorLocation(FinalLocation);

	FVector FinalLocDist = FinalLocation - MurdockCurLocation;
	if (FinalLocDist.IsNearlyZero(1.0f))
	{
		MurdockCharacter->Respawn(MurdockCurLocation);
		MurdockCharacter->SetCharacterCollisionType(ECharacterCollisionType::Normal);
		Controller->Possess(MurdockCharacter);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASS_RespawnTankPlayer::RespawnMurdockCharacter, 0.016f, false);
	}
}
