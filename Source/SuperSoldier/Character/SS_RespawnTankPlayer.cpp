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
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
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
		FollowCamera->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		FollowCamera->bUsePawnControlRotation = false;

		RespawnEndCameraRelativeLocation = FollowCamera->GetRelativeLocation();
		RespawnStartCameraRelativeLocation = RespawnEndCameraRelativeLocation;
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

	// Niagara
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TrailNiagaraRef(TEXT("/Game/RocketAttackFX/FX/Vehicle/NS_RocketVehicle_Missile.NS_RocketVehicle_Missile"));
	
	TrailNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailNiagara"));
	TrailNiagara->SetupAttachment(RootComponent);
	TrailNiagara->SetAsset(TrailNiagaraRef.Object);
	TrailNiagara->SetFloatParameter(TEXT("User.Thrust_Radius"), 20.0f);
	TrailNiagara->SetFloatParameter(TEXT("User.Thrust_Size"), 8.0f);
	TrailNiagara->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	TrailNiagara->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	TrailNiagara->bAutoActivate = false;
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
		}), 0.8f, false);

		NetMulticastEndNiagaraEffect();
	}

	else 
	{
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

	else
	{
		TrailNiagara->SetActive(true);

		// If Locally Controlled
		if (IsLocallyControlled())
		{
			SetActorTickEnabled(true);

			APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
			if (PlayerController)
			{
				EnableInput(PlayerController);
			}

			FollowCamera->SetRelativeLocation(RespawnStartCameraRelativeLocation);
		}
	}
}

void ASS_RespawnTankPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Camera Shake
	FStringClassReference LandingCameraLocationShakePath(TEXT("/Game/SuperSoldier/Camera/BP_LandingCameraShake.BP_LandingCameraShake_C"));
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

	FVector NewLocation = FMath::Lerp(FollowCamera->GetRelativeLocation(), RespawnEndCameraRelativeLocation, 0.1f);

	if (FollowCamera->GetRelativeLocation().Equals(NewLocation, 1.0f))
	{
		FollowCamera->SetRelativeLocation(RespawnEndCameraRelativeLocation);
		SetActorTickEnabled(false);
	}
	else
	{
		FollowCamera->SetRelativeLocation(NewLocation);
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
	FVector FinalLocation = FMath::Lerp(MurdockCurLocation, RespawnEndLocation, 0.075f);
	MurdockCharacter->SetActorLocation(FinalLocation);

	FVector FinalLocDist = RespawnEndLocation - MurdockCharacter->GetActorLocation();
	if (FinalLocDist.IsNearlyZero(3.0f))
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

void ASS_RespawnTankPlayer::SetRespawnStartCameraLocation(const FVector_NetQuantize& NewLocation)
{
	// SetCameraRelative Location
	FVector NewRelativeLocation = NewLocation - RootComponent->GetComponentLocation();
	RespawnStartCameraRelativeLocation = NewRelativeLocation;
}

void ASS_RespawnTankPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASS_RespawnTankPlayer, RespawnStartCameraRelativeLocation);
}

void ASS_RespawnTankPlayer::NetMulticastEndNiagaraEffect_Implementation()
{
	if (!HasAuthority())
	{
		TrailNiagara->SetActive(false);
	}
}

void ASS_RespawnTankPlayer::ClientRpcStartCameraEffect_Implementation(ASSCharacterPlayer* RespawnCharacter)
{
	check(RespawnCharacter);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	MurdockCharacter = RespawnCharacter;

	PlayerController->SetViewTargetWithBlend(RespawnCharacter, 0.8f);
}