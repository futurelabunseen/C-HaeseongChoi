// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/SSCharacterControlData.h"

ASSCharacterPlayer::ASSCharacterPlayer()
{
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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Fire.AM_Fire"));
	if (FireMontageRef.Object)
	{
		FireMontage = FireMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Fire.IA_Fire"));
	if (FireActionRef.Object)
	{
		FireAction = FireActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ThrowMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Throw.AM_Throw"));
	if (ThrowMontageRef.Object)
	{
		ThrowMontage = ThrowMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ThrowActionRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Grenade.IA_Grenade"));
	if (ThrowActionRef.Object)
	{
		ThrowAction = ThrowActionRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairWidgetRef(
		TEXT("/Game/SuperSoldier/UI/HUD.HUD_C"));

	if (CrosshairWidgetRef.Class)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetRef.Class);
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
	Super::SetCharacterControlData(CharacterControlData);

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

		// 몽타주가 종료가 될 때 함수가 호출되게 선언
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
	
	// 몽타주가 종료가 될 때 함수가 호출되게 선언
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
