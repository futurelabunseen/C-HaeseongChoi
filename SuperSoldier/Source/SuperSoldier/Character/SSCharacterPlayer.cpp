// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"

ASSCharacterPlayer::ASSCharacterPlayer()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bSprint = false;
	bAiming = false;
	bThrowing = false;

	// Input Action & Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(
		TEXT("/Game/SuperSoldier/Input/IMC_Normal.IMC_Normal"));
	if (InputMappingContextRef.Object)
	{
		NormalInputMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Move.IA_Move"));
	if (InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Look.IA_Look"));
	if (InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Sprint.IA_Sprint"));
	if (InputActionSprintRef.Object)
	{
		SprintAction = InputActionSprintRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAimRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Aim.IA_Aim"));
	if (InputActionAimRef.Object)
	{
		AimAction = InputActionAimRef.Object;
	}

	// Fire Input Action
	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Fire.IA_Fire"));
	if (FireActionRef.Object)
	{
		FireAction = FireActionRef.Object;
	}

	// Throw Throw Action
	static ConstructorHelpers::FObjectFinder<UInputAction> ThrowActionRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Grenade.IA_Grenade"));
	if (ThrowActionRef.Object)
	{
		ThrowAction = ThrowActionRef.Object;
	}

	// Widget (maybe Transfer to somewhere)
	static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairWidgetRef(
		TEXT("/Game/SuperSoldier/UI/HUD.HUD_C"));

	if (CrosshairWidgetRef.Class)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetRef.Class);
	}

	// Character Control Data
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

void ASSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Sprint);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Aim);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Fire);
	EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Throw);
}

void ASSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(NormalInputMappingContext, 0);
	}

	if (CrosshairWidget)
	{
		CrosshairWidget->AddToViewport();
		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ASSCharacterPlayer::SetCharacterControlData(const USSCharacterControlData* CharacterControlData)
{
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->MaxWalkSpeed = CharacterControlData->MaxWalkSpeed;

	CharacterControlData->bCrosshairVisibility ? 
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible) : 
		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeLocation(CharacterControlData->RelativeLocation);
}

void ASSCharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASSCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASSCharacterPlayer::Sprint(const FInputActionValue& Value)
{
	bSprint = Value.Get<bool>();

	// 총을 쏘거나 움직이고 있으면 달리기 불가능
	if (bFiring || bAiming)
	{
		return;
	}

	if (bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	}
}

void ASSCharacterPlayer::Aim(const FInputActionValue& Value)
{
	bAiming = Value.Get<bool>();

	if (bAiming)
	{
		SetCharacterControlData(*CharacterControlManager.Find(ECharacterControlType::Aiming));
	}
	else
	{
		SetCharacterControlData(*CharacterControlManager.Find(ECharacterControlType::Normal));

		if (!bSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		}
	}
}

void ASSCharacterPlayer::Fire(const FInputActionValue& Value)
{
	if (bAiming)
	{
		bFiring = true;

		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(FireMontage, AnimationSpeedRate);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ASSCharacterPlayer::EndFire);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, FireMontage);
	}
}

void ASSCharacterPlayer::EndFire(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	bFiring = false;
}

void ASSCharacterPlayer::Throw(const FInputActionValue& Value)
{
	bThrowing = true;

	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	const float AnimationSpeedRate = 1.25f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ThrowMontage, AnimationSpeedRate);
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ASSCharacterPlayer::EndThrow);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ThrowMontage);
}

void ASSCharacterPlayer::EndThrow(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	bThrowing = false;
	if (!bAiming && bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void ASSCharacterPlayer::AttackHitCheck()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + CameraRotation.Vector() * 5000.0f;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("Attack")), false, this);

	bool HitDetected = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CCHANNEL_SSACTION, TraceParams);

	if (HitDetected)
	{
	}

#if ENABLE_DRAW_DEBUG
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DrawColor, false, 5.0f);
#endif
}
