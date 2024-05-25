// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterPlayer.h"
#include "SuperSoldier.h"
#include "EngineUtils.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "SSCharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Core/SSGameInstance.h"
#include "Engine/DamageEvents.h"
#include "Weapon/SSWeaponComponent.h"
#include "Strata/SSStratagemManager.h"
#include "Strata/SSStrataIndicator.h"
#include "UI/SSUserPlayWidget.h"
#include "TimerManager.h"

ASSCharacterPlayer::ASSCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 0.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bAiming = false;
	bSprintKeyPressing = false;
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

	// Register Stratagem
	USSGameInstance* SSGameInstance = Cast<USSGameInstance>(GetGameInstance());
	USSStratagemManager* StratagemManager = SSGameInstance->GetStratagemManager();
	USSStratagem* DefaultStratagem = StratagemManager->GetStratagem(FName(TEXT("PrecisionStrike")));
	if (DefaultStratagem)
	{
		AvailableStratagems.Add(std::make_pair(FName(TEXT("PrecisionStrike")), DefaultStratagem));
	}

	// If Locally Controlled
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		if (PlayerController)
		{
			EnableInput(PlayerController);
		}

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(NormalInputMappingContext, 0);
		}
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
	if (!bAiming && bSprintKeyPressing && GetAnyMontagePlaying(TargetMontage) == false)
	{
		SetSprintToMovementComponent(true);
	}
}

void ASSCharacterPlayer::SetCharacterControlData(const USSCharacterControlData* CharacterControlData)
{
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SocketOffset = CharacterControlData->RelativeLocation;
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

	FRotator DeltaRotation = GetControlRotation() - GetActorRotation();
	DeltaRotation.Normalize();

	// YawDelta = DeltaRotation.Yaw;
	// PitchDelta = DeltaRotation.Pitch;
	
	// ServerRpcSetYawPitchDelta(YawDelta, PitchDelta);

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASSCharacterPlayer::ClientRpcSetYawPitchDelta_Implementation(ASSCharacterPlayer* CharacterToSet, float NewYawDelta, float NewPitchDelta)
{
	CharacterToSet->YawDelta = NewYawDelta;
	CharacterToSet->PitchDelta = NewPitchDelta;
}

void ASSCharacterPlayer::ServerRpcSetYawPitchDelta_Implementation(float NewYawDelta, float NewPitchDelta)
{
	YawDelta = NewYawDelta;
	PitchDelta = NewPitchDelta;

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (ASSCharacterPlayer* OtherPlayer = Cast<ASSCharacterPlayer>(PlayerController->GetPawn()))
			{
				OtherPlayer->ClientRpcSetYawPitchDelta(this, NewYawDelta, NewPitchDelta);
			}
		}
	}
}

void ASSCharacterPlayer::SetSprintToMovementComponent(bool bNewSprint)
{
	USSCharacterMovementComponent* SSCharacterMovement = Cast<USSCharacterMovementComponent>(GetCharacterMovement());
	SSCharacterMovement->SetSprint(bNewSprint);
}

void ASSCharacterPlayer::AttemptSprint()
{
	if(!bAiming && bSprintKeyPressing && !GetAnyMontagePlaying())
	{
		SetSprintToMovementComponent(true);
	}
}

void ASSCharacterPlayer::Sprint(const FInputActionValue& Value)
{
	bSprintKeyPressing = Value.Get<bool>();
	if (bSprintKeyPressing)
	{
		AttemptSprint();
	}
	else
	{
		SetSprintToMovementComponent(false);
	}
}

void ASSCharacterPlayer::Aim(const FInputActionValue& Value)
{
	bAiming = Value.Get<bool>();
	OnAiming.Broadcast(bAiming);
	SetAimingToMovementComponent(bAiming);

	if (bAiming)
	{
		SetCharacterControlData(*CharacterControlManager.Find(ECharacterControlType::Aiming));
		SetSprintToMovementComponent(false);

	}
	else
	{
		SetCharacterControlData(*CharacterControlManager.Find(ECharacterControlType::Normal));
		AttemptSprint();
	}
}

void ASSCharacterPlayer::SetAimingToMovementComponent(bool bNewAiming)
{
	USSCharacterMovementComponent* SSCharacterMovement = Cast<USSCharacterMovementComponent>(GetCharacterMovement());
	SSCharacterMovement->SetAiming(bNewAiming);
}

void ASSCharacterPlayer::Fire(const FInputActionValue& Value)
{
	if (!GetAnyMontagePlaying(StrataReadyMontage) && bReadyForThrowingStrata)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(StrataThrowMontage, AnimationSpeedRate);

		FRotator ControlRotation = GetControlRotation();
		FRotator CurRotation = GetActorRotation();
		SetActorRotation(FRotator(CurRotation.Pitch, ControlRotation.Yaw, CurRotation.Roll));

		ServerRpcStrataThrow();

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

		ServerRpcFire();
	}
}

void ASSCharacterPlayer::Throw(const FInputActionValue& Value)
{
	if (!GetAnyMontagePlaying())
	{
		SetSprintToMovementComponent(false);

		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(ThrowMontage, AnimationSpeedRate);

		ServerRpcThrow();

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
		OnCalling.Broadcast(bCalling);

		if (bCalling)
		{
			SetSprintToMovementComponent(false);

			const float AnimationSpeedRate = 1.0f;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(CallMontage, AnimationSpeedRate);

			ServerRpcCalling();

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
				ServerRpcEndCalling();
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
		bChangeMontageForThrowingStrata = false;

		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(StrataReadyMontage, AnimationSpeedRate);

		FName StrataNameToActivate = AvailableStratagems[SelectedStrataIndex].first;
		ServerRpcStrataReady(StrataNameToActivate);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ASSCharacterPlayer::EndStrata);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, StrataReadyMontage);
	}
	else
	{
		if (!bAiming && bSprintKeyPressing && GetAnyMontagePlaying(TargetMontage) == false)
		{
			SetSprintToMovementComponent(true);
		}
	}
}

void ASSCharacterPlayer::EndStrata(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	bReadyForThrowingStrata = false;

	// 다른 방법 필요
	// GetMesh()->UnHideBoneByName(TEXT("bot_hand"));
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
	if (!AvailableStratagems.IsEmpty())
	{
		// 사용 가능한 스트라타젬을 순회, 매칭
		for (int i = 0; i < AvailableStratagems.Num(); ++i)
		{
			if (AvailableStratagems.IsValidIndex(i))
			{
				USSStratagem* Stratagem = AvailableStratagems[i].second;

				const TArray<EStrataCommand> StrataCommandArr = Stratagem->GetCommandSequence();

				// 스트라타젬 커맨드 수보다, 현재 입력 커맨드 수가 많으면 검사할 필요가 없음
				if (InputSequence.Num() > StrataCommandArr.Num())
				{
					continue;
				}

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
						SelectedStrataIndex = i;

						UE_LOG(LogTemp, Log, TEXT("Reset InputSequence"))
						InputSequence.Reset();

						UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
						if (AnimInstance)
						{
							AnimInstance->Montage_JumpToSection(TEXT("End"), CallMontage);
							ServerRpcEndCalling();
						}

						bChangeMontageForThrowingStrata = true;
					}
				}
			}
		}
	}
	
	return bSuccessMatching;
}

void ASSCharacterPlayer::ProcessCommandInput(const FInputActionValue& Value)
{
	if (bCalling)
	{
		TranslateInput(Value);
		if (!MatchingInput())
		{
			UE_LOG(LogTemp, Log, TEXT("Matching Fail Reset InputSequence"))
			InputSequence.Reset();
		}
	}
}

void ASSCharacterPlayer::AttackHitCheck()
{
	if (MainWeapon)
	{
		FHitResult HitResult = MainWeapon->AttackHitCheck();
		if (IsLocallyControlled())
		{
			MainWeapon->ShowAttackEffect(HitResult);
		}

		if (HasAuthority() && HitResult.bBlockingHit)
		{
			FDamageEvent DamageEvent;
			const float AttackDamage = 30.0f;
			HitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}
	}
}

void ASSCharacterPlayer::PlaySoundEffect()
{
	MainWeapon->PlaySoundEffect();
}

void ASSCharacterPlayer::ReleaseThrowable()
{
	// Throw Actor
	if (CurStrataIndicator)
	{
		CurStrataIndicator->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurStrataIndicator->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f).Quaternion());

		UClass* StrataIndicatorClass = CurStrataIndicator.GetClass();
		UFunction* ThrowFunction = CurStrataIndicator->FindFunction(FName(TEXT("Throw")));

		if (ThrowFunction)
		{
			// Throw Camera Direction Vector
			FVector CameraLocation;
			FRotator CameraRotation;
			GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
			
			FVector ThrowPos = CameraLocation + CameraRotation.Vector() * 2000.0f;
			FVector ThrowDirection = ThrowPos - CurStrataIndicator->GetActorLocation();
			ThrowDirection.Normalize();

			CurStrataIndicator->ProcessEvent(ThrowFunction, &ThrowDirection);
		}
	}
}

float ASSCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ClientRpcPlayAnimation(this, HitReactMontage);
	RpcPlayAnimation(HitReactMontage);

	return Result;
}

void ASSCharacterPlayer::SetupCharacterWidget(USSUserPlayWidget* InUserWidget)
{
	Super::SetupCharacterWidget(InUserWidget);

	if (InUserWidget)
	{
		OnAiming.AddUObject(InUserWidget, &USSUserPlayWidget::UpdateCrossHair);
		OnCalling.AddUObject(InUserWidget, &USSUserPlayWidget::ShowStratagemList);
	}
}

void ASSCharacterPlayer::RpcPlayAnimation(UAnimMontage* MontageToPlay)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				ASSCharacterPlayer* OtherPlayer = Cast<ASSCharacterPlayer>(PlayerController->GetPawn());

				if (OtherPlayer)
				{
					OtherPlayer->ClientRpcPlayAnimation(this, MontageToPlay);
				}
			}
		}
	}
}

void ASSCharacterPlayer::RpcJumpToSection(UAnimMontage* MontageToPlay, FName SectionName)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				ASSCharacterPlayer* OtherPlayer = Cast<ASSCharacterPlayer>(PlayerController->GetPawn());

				if (OtherPlayer)
				{
					OtherPlayer->ClientRpcJumpToSection(this, MontageToPlay, SectionName);
				}
			}
		}
	}
}

void ASSCharacterPlayer::ClientRpcJumpToSection_Implementation(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay, FName SectionName)
{
	if (CharacterToPlay)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = CharacterToPlay->GetMesh()->GetAnimInstance();
		FName CurSection = AnimInstance->Montage_GetCurrentSection(MontageToPlay);

		if (CurSection != NAME_None)
		{
			bool bNotAlreadyPlaying = CurSection.Compare(SectionName) != 0;

			if (bNotAlreadyPlaying)
			{
				AnimInstance->Montage_JumpToSection(SectionName, CallMontage);
			}
		}
	}
}

void ASSCharacterPlayer::ClientRpcPlayAnimation_Implementation(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay)
{
	if (CharacterToPlay)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = CharacterToPlay->GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MontageToPlay, AnimationSpeedRate);
	}
}

bool ASSCharacterPlayer::ServerRpcFire_Validate()
{
	return true;
}

void ASSCharacterPlayer::ServerRpcFire_Implementation()
{
	FTimerHandle HitCheckTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HitCheckTimerHandle, this, &ASSCharacterPlayer::AttackHitCheck, 0.07f, false);

	RpcPlayAnimation(FireMontage);
}

bool ASSCharacterPlayer::ServerRpcNotifyFireHit_Validate(const FHitResult& HitResult)
{
	return true;
}

void ASSCharacterPlayer::ServerRpcNotifyFireHit_Implementation(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor))
	{
		const float AcceptCheckDistance = 300.0f;

		const FVector HitLocation = HitResult.Location;
		const FBox HitBox = HitActor->GetComponentsBoundingBox();
		const FVector ActorBoxCenter = (HitBox.Min + HitBox.Max) * 0.5f;

		if (FVector::DistSquared(HitLocation, ActorBoxCenter) <= AcceptCheckDistance * AcceptCheckDistance)
		{
			FDamageEvent DamageEvent;
			const float AttackDamage = 30.0f;
			HitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}
	}
}

bool ASSCharacterPlayer::ServerRpcThrow_Validate()
{
	return true;
}

void ASSCharacterPlayer::ServerRpcThrow_Implementation()
{
	const float AnimationSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ThrowMontage, AnimationSpeedRate);

	RpcPlayAnimation(ThrowMontage);
}

bool ASSCharacterPlayer::ServerRpcCalling_Validate()
{
	return true;
}

void ASSCharacterPlayer::ServerRpcCalling_Implementation()
{
	RpcPlayAnimation(CallMontage);
}

bool ASSCharacterPlayer::ServerRpcEndCalling_Validate()
{
	return true;
}

void ASSCharacterPlayer::ServerRpcEndCalling_Implementation()
{
	RpcJumpToSection(CallMontage, TEXT("End"));
}

bool ASSCharacterPlayer::ServerRpcStrataReady_Validate(const FName& StratagemName)
{
	return true;
}

void ASSCharacterPlayer::ServerRpcStrataReady_Implementation(const FName& StratagemName)
{
	RpcPlayAnimation(StrataReadyMontage);

	CurStrataIndicator = GetWorld()->SpawnActor<ASSStrataIndicator>(ASSStrataIndicator::StaticClass());

	USSGameInstance* SSGameInstance = Cast<USSGameInstance>(GetGameInstance());
	USSStratagemManager* StratagemManager = SSGameInstance->GetStratagemManager();
	USSStratagem* SelectedStratagem = StratagemManager->GetStratagem(StratagemName);
	CurStrataIndicator->SetStratagem(SelectedStratagem);

	if (CurStrataIndicator)
	{
		USkeletalMeshComponent* PlayerSkeletalMesh = GetMesh();
		if (PlayerSkeletalMesh)
		{
			FName SocketName = TEXT("Socket_StrataIndicator");
			FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepRelative,
				true);

			CurStrataIndicator->AttachToComponent(PlayerSkeletalMesh, AttachmentRules, SocketName);
		}
	}
}

bool ASSCharacterPlayer::ServerRpcStrataThrow_Validate()
{
	return true;
}

void ASSCharacterPlayer::ServerRpcStrataThrow_Implementation()
{
	FRotator ControlRotation = GetControlRotation();
	FRotator CurRotation = GetActorRotation();
	SetActorRotation(FRotator(CurRotation.Pitch, ControlRotation.Yaw, CurRotation.Roll));

	const float AnimationSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(StrataThrowMontage, AnimationSpeedRate);

	RpcPlayAnimation(StrataThrowMontage);
}

void ASSCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
