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
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
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

	// SFX
	static ConstructorHelpers::FObjectFinder<USoundBase> FallingSoundRef(TEXT("/Game/SuperSoldier/Sounds/SC_Falling.SC_Falling"));
	if (FallingSoundRef.Object)
	{
		FallingSound = FallingSoundRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> LandingSoundRef(TEXT("/Game/SuperSoldier/Sounds/SC_Landing.SC_Landing"));
	if (LandingSoundRef.Object)
	{
		LandingSound = LandingSoundRef.Object;
	}

	LerpCharacterAlpha = 0.0f;
}

void ASS_RespawnTankPlayer::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	GetCharacterMovement()->SetMovementMode(MOVE_None);

	if (HasAuthority())
	{
		// 충돌을 비활성화
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 리스폰할 캐릭터를 게임에 등장시키고 Detach
		ASSCharacterPlayer* PlayerCharacter = Cast<ASSCharacterPlayer>(MurdockCharacter);
		check(PlayerCharacter);
		PlayerCharacter->SetActorHiddenInGame(false);
		PlayerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 충분한 시간 뒤에 캐릭터를 리스폰, 카메라 효과가 보여지도록 Client RPC를 호출
		FTimerHandle CallFuncTimer;
		const float RespawnDelay = 0.8f;
		GetWorld()->GetTimerManager().SetTimer(CallFuncTimer, FTimerDelegate::CreateLambda([&]() {
			APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
			PlayerController->SetViewTargetWithBlend(
				MurdockCharacter, 
				0.6f, 
				EViewTargetBlendFunction::VTBlend_EaseIn,
				2.0f);
			RespawnMurdockCharacter();
		}), RespawnDelay, false);

		// Multicast RPC로 랜딩 후, 이펙트 처리
		NetMulticastSettingEffectOnLand();
	}

	else 
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		if (PlayerController)
		{
			// 입력을 비활성화
			DisableInput(PlayerController);

			// 카메라 흔들림 연출 수행
			PlayerController->ClientStartCameraShake(LandingCameraLocationShakeClass);

			// 추락 사운드가 더이상 안나게 설정
			FallingSoundComponent->Stop();
			UGameplayStatics::SpawnSound2D(GetWorld(), LandingSound, 1.0f, 1.0f, 2.2f);
		}
	}
}

void ASS_RespawnTankPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (HasAuthority())
	{
		// 착지 후 Possess할 플레이어 캐릭터를 월드에 스폰하고 소켓 위치에 부착한다.
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		MurdockCharacter = GetWorld()->SpawnActor<ASS_MurdockPlayer>(ASS_MurdockPlayer::StaticClass(), ActorSpawnParameters);

		check(MurdockCharacter);

		FName SocketName = TEXT("Socket_MurdockCharacter");
		MurdockCharacter->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName);

		// 착지하기 전까진 보여질 필요가 없기 때문에 게임에서 안보이게한다.
		MurdockCharacter->SetActorHiddenInGame(true);
		MurdockCharacter->SetCharacterCollisionType(ECharacterCollisionType::NoCollision);
	}
	else
	{
		TrailNiagara->SetActive(true);

		// If Locally Controlled
		if (IsLocallyControlled())
		{
			APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
			if (PlayerController)
			{
				EnableInput(PlayerController);
			}

			FollowCamera->SetRelativeLocation(RespawnStartCameraRelativeLocation);
			FallingSoundComponent = UGameplayStatics::SpawnSound2D(
				GetWorld(), 
				FallingSound);
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
		SetActorTickEnabled(true);
		Controller->SetControlRotation(FRotator::ZeroRotator);
	}
}

void ASS_RespawnTankPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector NewLocation = FMath::Lerp(FollowCamera->GetRelativeLocation(), RespawnEndCameraRelativeLocation, 0.1f);

	// 충분히 비슷하다면 카메라 효과를 중지
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

	LerpCharacterAlpha = FMath::Clamp(LerpCharacterAlpha + 0.048f, 0.0f, 1.0f);

	FName RespawnStartLocationSocketName = TEXT("Socket_MurdockCharacter");
	FName RespawnEndLocationSocketName = TEXT("Socket_RespawnComplete");

	// 소켓 위치를 이용해 리스폰 시작 위치와 끝 위치를 설정 
	FVector RespawnStartLocation = GetMesh()->GetSocketTransform(RespawnStartLocationSocketName).GetLocation();
	FVector RespawnEndLocation = GetMesh()->GetSocketTransform(RespawnEndLocationSocketName).GetLocation();
	FVector LocationDist = RespawnEndLocation - RespawnStartLocation;
	RespawnEndLocation += LocationDist * 0.5f;

	// 리스폰 시작 위치에서 리스폰 최종 위치로 보간해 등장 연출
	FVector FinalLocation = FMath::InterpEaseInOut(RespawnStartLocation, RespawnEndLocation, LerpCharacterAlpha, 4.0f);
	LerpCharacterAlpha >= 0.85f ? FinalLocation = RespawnEndLocation : FinalLocation;
	MurdockCharacter->SetActorLocation(FinalLocation);

	// 등장 연출이 끝나면 컨트롤러가 리스폰한 캐릭터를 Possess하게 한다.
	if (LerpCharacterAlpha >= 1.0f)
	{
		MurdockCharacter->SetCharacterCollisionType(ECharacterCollisionType::Normal);
		MurdockCharacter->SetActorLocation(RespawnEndLocation);
		MurdockCharacter->Respawn(MurdockCharacter->GetActorLocation());
		GetController()->Possess(MurdockCharacter);
		SetLifeSpan(2.0f);
	}
	// 일정 시간 뒤, 다시 실행되도록 타이머 설정
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

void ASS_RespawnTankPlayer::NetMulticastSettingEffectOnLand_Implementation()
{
	if (!HasAuthority())
	{
		TrailNiagara->SetActive(false);

		if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), LandingSound, GetActorLocation(), FRotator::ZeroRotator, 1.0f, 1.0f, 2.2f);
		}
	}
}

void ASS_RespawnTankPlayer::ClientRpcStartCameraEffect_Implementation(ASSCharacterPlayer* RespawnCharacter)
{
	check(RespawnCharacter);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	MurdockCharacter = RespawnCharacter;

	PlayerController->SetViewTargetWithBlend(MurdockCharacter, 0.6f);
}