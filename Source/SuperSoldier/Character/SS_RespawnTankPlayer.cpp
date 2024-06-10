// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_RespawnTankPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Character/SS_MurdockPlayer.h"
#include "Kismet/GameplayStatics.h"
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

	bStartLerpCamera = false;
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

		FTimerHandle CallFuncTimer;
		GetWorld()->GetTimerManager().SetTimer(CallFuncTimer, FTimerDelegate::CreateLambda([&]() {
			ClientRpcStartCameraEffect(MurdockCharacter);
			RespawnMurdockCharacter();
		}), 0.5f, false);
	}

	if (IsLocallyControlled())
	{
		SS_LOG(LogSSNetwork, Log, TEXT("Client Landing"))

		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		if (PlayerController)
		{
			// Disable Input
			DisableInput(PlayerController);

			// Play the Camera Shake
			PlayerController->ClientStartCameraShake(LandingCameraLocationShakeClass);
		}
	}
}

void ASS_RespawnTankPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (HasAuthority())
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		MurdockCharacter = GetWorld()->SpawnActor<ASS_MurdockPlayer>(ASS_MurdockPlayer::StaticClass(), ActorSpawnParameters);

		check(MurdockCharacter);

		FName SocketName = TEXT("Socket_MurdockCharacter");
		MurdockCharacter->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName);

		MurdockCharacter->SetActorHiddenInGame(true);
		MurdockCharacter->SetCharacterCollisionType(ECharacterCollisionType::NoCollision);
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

void ASS_RespawnTankPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Camera Shake
	FStringClassReference LandingCameraLocationShakePath(TEXT("/Game/SuperSoldier/Camera/BP_LandingCameraLocationShake.BP_LandingCameraLocationShake_C"));
	UClass* LandingCameraLocationShakeRef = LandingCameraLocationShakePath.TryLoadClass<UCameraShakeBase>();
	if (LandingCameraLocationShakeRef)
	{
		LandingCameraLocationShakeClass = LandingCameraLocationShakeRef;
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

	if (bStartLerpCamera)
	{
		LerpCamera(DeltaSeconds);
	}
}

void ASS_RespawnTankPlayer::RespawnMurdockCharacter()
{
	check(MurdockCharacter);

	SetRespawnMurdockLocation();
}

void ASS_RespawnTankPlayer::SetRespawnMurdockLocation()
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

	FVector FinalLocDist = RespawnEndLocation - MurdockCharacter->GetActorLocation();
	if (FinalLocDist.IsNearlyZero(1.0f))
	{
		Controller->Possess(MurdockCharacter);
		MurdockCharacter->Respawn(MurdockCharacter->GetActorLocation());
		MurdockCharacter->SetCharacterCollisionType(ECharacterCollisionType::Normal);
		SetLifeSpan(2.0f);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASS_RespawnTankPlayer::RespawnMurdockCharacter, 0.016f, false);
	}
}

void ASS_RespawnTankPlayer::LerpCamera(float DeltaSeconds)
{
	LerpAlpha = FMath::Clamp(LerpAlpha + DeltaSeconds * 1.5f, 0.0f, 1.0f);
	FVector StartCameraLocation = CameraLerpStartTransform.GetLocation();
	FRotator StartCameraRotation = CameraLerpStartTransform.GetRotation().Rotator();

	const UCameraComponent* MurdockCamera = MurdockCharacter->GetFollowCamera();
	FVector EndCameraLocation = MurdockCamera->GetComponentLocation();
	FRotator EndCameraRotation = MurdockCamera->GetComponentRotation();

	FVector NewCurCameraLocation = FMath::Lerp(StartCameraLocation, EndCameraLocation, LerpAlpha);
	FRotator NewCurCameraRotation = FMath::Lerp(StartCameraRotation, EndCameraRotation, LerpAlpha);

	FollowCamera->SetWorldLocation(NewCurCameraLocation);
	FollowCamera->SetWorldRotation(NewCurCameraRotation);
}

void ASS_RespawnTankPlayer::ClientRpcStartCameraEffect_Implementation(ASSCharacterPlayer* RespawnCharacter)
{
	check(RespawnCharacter);

	SetActorTickEnabled(true);
	bStartLerpCamera = true;
	LerpAlpha = 0.0f;
	MurdockCharacter = RespawnCharacter;
	CameraLerpStartTransform = FollowCamera->GetComponentTransform();
}