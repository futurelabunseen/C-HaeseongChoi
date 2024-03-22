// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

void ASSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Sprint);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Aim);
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

	if (bAiming)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SpeedChangeTimerHandle, this,
		&ASSCharacterPlayer::UpdateMaxWalkSpeed, 0.02f, true);
}

void ASSCharacterPlayer::UpdateMaxWalkSpeed()
{
	float TargetSpeed = bSprint ? 600.0f : 400.0f;
	float CurrentSpeed = GetCharacterMovement()->MaxWalkSpeed;
	const float SpeedChangeTimePerSecond = 15.0f;

	// 현재 최대 이동속도와 캐릭터가 가져야할 최대 이동속도를 보간하는 코드
	float NewSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, 
		GetWorld()->GetDeltaSeconds(), SpeedChangeTimePerSecond);

	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

	// 만약 오차범위 0.5f 미만으로 같다면 타이머를 종료
	if (FMath::IsNearlyEqual(NewSpeed, TargetSpeed, 0.5f))
	{
		GetWorld()->GetTimerManager().ClearTimer(SpeedChangeTimerHandle);
	}
}

void ASSCharacterPlayer::Aim(const FInputActionValue& Value)
{
	bAiming = Value.Get<bool>();

	if (bAiming)
	{
		UE_LOG(LogTemp, Log, TEXT("Aiming"));
		CameraBoom->TargetArmLength = 120.0f;
		CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 70.0f));

		GetCharacterMovement()->MaxWalkSpeed = 400.0f;

		// Pawn
		bUseControllerRotationYaw = false;

		// Movement
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Release Aim"));
		CameraBoom->TargetArmLength = 400.0f;
		CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));

		if (bSprint)
		{
			GetWorld()->GetTimerManager().SetTimer(SpeedChangeTimerHandle, this,
				&ASSCharacterPlayer::UpdateMaxWalkSpeed, 0.02f, true);
		}

		bUseControllerRotationYaw = false;

		// Movement
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	// 카메라 위치 변화 필요
}