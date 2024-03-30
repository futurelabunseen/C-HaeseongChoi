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
#include "Engine/DamageEvents.h"

#include "Core/SSGameInstance.h"
#include "Strata/SSStratagem.h"
#include "Strata/SSStratagemManager.h"

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
	bReadyForThrowingStrata = false;
	bChangeMontageForThrowingStrata = false;

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

	// Call Throw Action
	static ConstructorHelpers::FObjectFinder<UInputAction> CallActionRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Call.IA_Call"));
	if (CallActionRef.Object)
	{
		CallAction = CallActionRef.Object;
	}

	// Strata Command Action
	static ConstructorHelpers::FObjectFinder<UInputAction> CommandActionRef(
		TEXT("/Game/SuperSoldier/Input/Actions/IA_Command.IA_Command"));
	if (CommandActionRef.Object)
	{
		CommandAction = CommandActionRef.Object;
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
	EnhancedInputComponent->BindAction(CallAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Call);
	EnhancedInputComponent->BindAction(CommandAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::ProcessCommandInput);
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

	// Register Stratagem
	USSGameInstance* SSGameInstance = Cast<USSGameInstance>(GetGameInstance());
	USSStratagemManager* StratagemManager = SSGameInstance->GetStratagemManager();
	USSStratagem* DefaultStratagem = StratagemManager->GetStratagem(FName(TEXT("Stratagem")));
	if (DefaultStratagem)
	{
		AvailableStratagems.Add(DefaultStratagem);
	}
}

bool ASSCharacterPlayer::GetAnyMontagePlaying(UAnimMontage* FilterMontage)
{
	bool bRet = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (FilterMontage == NULL)
	{
		bRet = AnimInstance->IsAnyMontagePlaying();
	}
	else
	{
		UAnimMontage* CurrentActivateMontage = AnimInstance->GetCurrentActiveMontage();
		UClass* CurMontageClass = CurrentActivateMontage->StaticClass();
		UClass* FilterMontageClass = FilterMontage->StaticClass();

		bRet = AnimInstance->IsAnyMontagePlaying() &&
			CurMontageClass != FilterMontageClass;
	}
	return bRet;
}

void ASSCharacterPlayer::AttemptSprintEndDelegate(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	if (!bAiming && bSprint && GetAnyMontagePlaying(TargetMontage) == false)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
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

bool ASSCharacterPlayer::AttemptSprint()
{
	if(!bAiming && bSprint && !GetAnyMontagePlaying())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		return true;
	}
	return false;
}

void ASSCharacterPlayer::Sprint(const FInputActionValue& Value)
{
	bSprint = Value.Get<bool>();

	if (AttemptSprint() == false)
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

		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		AttemptSprint();
	}
}

void ASSCharacterPlayer::Fire(const FInputActionValue& Value)
{
	if (bReadyForThrowingStrata)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(StrataThrowMontage, AnimationSpeedRate);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ASSCharacterPlayer::AttemptSprintEndDelegate);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, StrataThrowMontage);
		return;
	}

	// 애니메이션 몽타주가 재생 중이 아니고, 조준 중이라면 격발
	if (!GetAnyMontagePlaying() && bAiming)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(FireMontage, AnimationSpeedRate);
	}
}

void ASSCharacterPlayer::Throw(const FInputActionValue& Value)
{
	if (!GetAnyMontagePlaying())
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;

		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(ThrowMontage, AnimationSpeedRate);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ASSCharacterPlayer::AttemptSprintEndDelegate);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, ThrowMontage);
	}
}

void ASSCharacterPlayer::Call(const FInputActionValue& Value)
{
	if (!GetAnyMontagePlaying(CallMontage))
	{
		bCalling = Value.Get<bool>();

		if (bCalling)
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;

			const float AnimationSpeedRate = 1.0f;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(CallMontage, AnimationSpeedRate);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &ASSCharacterPlayer::EndCalling);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, CallMontage);
		}

		else
		{
			// 현재 재생중인 섹션이 End가 아니면 End 섹션을 재생
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			FName CurSection = AnimInstance->Montage_GetCurrentSection(CallMontage);
			bool bNotAlreadyPlaying = CurSection.Compare(FName(TEXT("End"))) != 0;

			if (bNotAlreadyPlaying)
			{
				AnimInstance->Montage_JumpToSection(TEXT("End"), CallMontage);
			}

			InputSequence.Reset();
		}
	}
}

void ASSCharacterPlayer::EndCalling(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	if (bChangeMontageForThrowingStrata)
	{
		bReadyForThrowingStrata = true;
		
		// 다른 방법 필요
		GetMesh()->HideBoneByName(TEXT("bot_hand"), EPhysBodyOp::PBO_None);

		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(StrataReadyMontage, AnimationSpeedRate);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ASSCharacterPlayer::EndStrata);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, StrataReadyMontage);
	}
	else
	{
		if (!bAiming && bSprint && GetAnyMontagePlaying(TargetMontage) == false)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}
	}
}

void ASSCharacterPlayer::EndStrata(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	bReadyForThrowingStrata = false;

	// 다른 방법 필요
	GetMesh()->UnHideBoneByName(TEXT("bot_hand"));
}

void ASSCharacterPlayer::TranslateInput(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();
	EStrataCommand StrataCommand = EStrataCommand::NONE;

	if (InputValue.X)
	{
		if (InputValue.X > 0)
		{
			StrataCommand = EStrataCommand::RIGHT;
		}
		else
		{
			StrataCommand = EStrataCommand::LEFT;
		}
	}

	else
	{
		if (InputValue.Y > 0)
		{
			StrataCommand = EStrataCommand::UP;
		}
		else
		{
			StrataCommand = EStrataCommand::DOWN;
		}
	}

	InputSequence.Add(StrataCommand);

	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EStrataCommand"), true);
	check(EnumPtr);

	UE_LOG(LogTemp, Log, TEXT("============CurrentInputSequence============="))
	for (const EStrataCommand& Cmd : InputSequence)
	{
		FString CommandString = EnumPtr->GetDisplayNameTextByValue(static_cast<uint32>(Cmd)).ToString();

		UE_LOG(LogTemp, Log, TEXT("%s"), *CommandString);
	}
	UE_LOG(LogTemp, Log, TEXT("============================================="))

#ifdef DEBUG_STRATAINPUT
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EStrataCommand"), true);
	check(EnumPtr);

	FString CommandString = EnumPtr->GetDisplayNameTextByValue(static_cast<uint32>(StrataCommand)).ToString();
	UE_LOG(LogTemp, Log, TEXT("%s"), *CommandString);
#endif // DEBUG_STRATAINPUT
}

bool ASSCharacterPlayer::MatchingInput()
{
	bool bSuccessMatching = false;
	if (AvailableStratagems.IsEmpty() == false)
	{
		// 사용 가능한 스트라타젬을 순회, 매칭
		for (USSStratagem* Stratagem : AvailableStratagems)
		{
			const TArray<EStrataCommand> StrataCommandArr = Stratagem->GetCommandSequence();

			// 스트라타젬 커맨드 수보다, 현재 입력 커맨드 수가 많으면 검사할 필요가 없음
			if (InputSequence.Num() > StrataCommandArr.Num()) continue;

			// 입력 커맨드 수만큼 순회 하면서, 스트라타젬 커맨드와 비교
			bool bAllInputCommandMatching = true;
			for (int j = 0; j < InputSequence.Num(); ++j)
			{
				if (InputSequence[j] != StrataCommandArr[j])
				{
					bAllInputCommandMatching = false;
					break;
				}
			}

			// 모든 커맨드가 매칭이 성공했을 경우
			if (bAllInputCommandMatching)
			{
				bSuccessMatching = true;

				// 모든 커맨드가 매칭 성공 했으며, 입력 수와 스트라타젬 커맨드 수가 같으면
				// 스트라타젬을 발동, 입력 커맨드 배열을 비운다.
				int32 InputSequenceNum = InputSequence.Num();
				int32 StrataCommandArrNum = StrataCommandArr.Num();

				if (InputSequenceNum == StrataCommandArrNum)
				{
					UE_LOG(LogTemp, Log, TEXT("Matching Success!!!!"))
						Stratagem->ActivateStratagem();

					UE_LOG(LogTemp, Log, TEXT("Reset InputSequence"))
						InputSequence.Reset();

					UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
					if (AnimInstance)
					{
						AnimInstance->Montage_JumpToSection(TEXT("End"), CallMontage);
					}

					bChangeMontageForThrowingStrata = true;
				}
			}
		}
	}
	
	return bSuccessMatching;
}

void ASSCharacterPlayer::ProcessCommandInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Start ProcessCommandInput"))
	if (bCalling)
	{
		TranslateInput(Value);
	}

	if (!MatchingInput())
	{
		UE_LOG(LogTemp, Log, TEXT("Matching Fail Reset InputSequence"))
		InputSequence.Reset();
		UE_LOG(LogTemp, Log, TEXT("Reset Complete"))
	}
	UE_LOG(LogTemp, Log, TEXT("End ProcessCommandInput"))
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
		FDamageEvent DamageEvent;
		const float AttackDamage = 30.0f;
		HitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DrawColor, false, 5.0f);
#endif
}
