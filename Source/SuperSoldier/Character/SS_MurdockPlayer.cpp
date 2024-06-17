// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_MurdockPlayer.h"
#include "Core/SSGameInstance.h"
#include "Core/SSStatisticsManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SSCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Strata/SSStratagemManager.h"
#include "Strata/SSStrataIndicator.h"
#include "Weapon/SSWeaponComponent_Rifle.h"
#include "Character/SSCharacterControlData.h"
#include "Engine/DamageEvents.h"
#include "UI/SSUserPlayWidget.h"
#include "SuperSoldier.h"

ASS_MurdockPlayer::ASS_MurdockPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Pawn
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}

	// Capsule
	{
		GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	}

	// Movement
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	}

	// Camera
	{
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = 500.0f;
		CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 0.0f);
		CameraBoom->bUsePawnControlRotation = true;

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		FollowCamera->bUsePawnControlRotation = false;
	}

	// Look Section
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_Look.IA_Look"));
		if (InputActionLookRef.Object)
		{
			LookAction = InputActionLookRef.Object;
		}
	}

	// Mesh & AnimInstance
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
			TEXT("/Game/ParagonMurdock/Characters/Heroes/Murdock/Skins/A_Executioner/Mesh/Murdock_Executioner.Murdock_Executioner"));
		if (CharacterMeshRef.Object)
		{
			GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
		}

		static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/ABP_MurdockPlayer.ABP_MurdockPlayer_C"));
		if (AnimInstanceClassRef.Class)
		{
			GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
		}

		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	// Fire Section
	{
		// Fire Input Action
		static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_Fire.IA_Fire"));
		if (FireActionRef.Object)
		{
			FireAction = FireActionRef.Object;
		}

		// Fire Montage
		static ConstructorHelpers::FObjectFinder<UAnimMontage> FireMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Fire.AM_Fire"));
		if (FireMontageRef.Object)
		{
			FireMontage = FireMontageRef.Object;
		}
	}

	// Call Section
	{
		// Call Action
		static ConstructorHelpers::FObjectFinder<UInputAction> CallActionRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_Call.IA_Call"));
		if (CallActionRef.Object)
		{
			CallAction = CallActionRef.Object;
		}

		// Call Montage
		static ConstructorHelpers::FObjectFinder<UAnimMontage> CallMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Call.AM_Call"));
		if (CallMontageRef.Object)
		{
			CallMontage = CallMontageRef.Object;
		}
	}

	// Strata Section
	{
		bReadyForThrowingStrata = false;
		bChangeMontageForThrowingStrata = false;

		// Strata Command Action
		static ConstructorHelpers::FObjectFinder<UInputAction> CommandActionRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_Command.IA_Command"));
		if (CommandActionRef.Object)
		{
			CommandAction = CommandActionRef.Object;
		}

		// Strata Montage
		static ConstructorHelpers::FObjectFinder<UAnimMontage> StrataReadyMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_StrataReady.AM_StrataReady"));
		if (StrataReadyMontageRef.Object)
		{
			StrataReadyMontage = StrataReadyMontageRef.Object;
		}

		static ConstructorHelpers::FObjectFinder<UAnimMontage> StrataThrowMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_StrataThrow.AM_StrataThrow"));
		if (StrataThrowMontageRef.Object)
		{
			StrataThrowMontage = StrataThrowMontageRef.Object;
		}
	}

	// HitReact Montage & Sound
	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> HitReactMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_HitReact.AM_HitReact"));
		if (HitReactMontageRef.Object)
		{
			HitReactMontage = HitReactMontageRef.Object;
		}

		static ConstructorHelpers::FObjectFinder<USoundBase> MoanSoundRef(TEXT("/Game/ParagonMurdock/Characters/Heroes/Murdock/Sounds/SoundCues/Murdock_Effort_Pain_03_Dialogue_Cue.Murdock_Effort_Pain_03_Dialogue_Cue"));
		if (MoanSoundRef.Object)
		{
			MoanSound = MoanSoundRef.Object;
		}

		static ConstructorHelpers::FObjectFinder<USoundBase> DeadSoundRef(TEXT("/Game/ParagonMurdock/Characters/Heroes/Murdock/Sounds/SoundCues/Murdock_Effort_Death_02_Dialogue_Cue.Murdock_Effort_Death_02_Dialogue_Cue"));
		if (DeadSoundRef.Object)
		{
			DeadSound = DeadSoundRef.Object;
		}

	}

	// Respawn Montage
	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> RespawnMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Murdock/Animations/AM_Respawn.AM_Respawn"));
		if (RespawnMontageRef.Object)
		{
			RespawnMontage = RespawnMontageRef.Object;
		}
	}

	// Character Control Data
	{
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

	// Sprint Section
	{
		bSprintKeyPressing = false;
		static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_Sprint.IA_Sprint"));
		if (InputActionSprintRef.Object)
		{
			SprintAction = InputActionSprintRef.Object;
		}
	}

	// Aim Section
	{
		bAiming = false;

		static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAimRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_Aim.IA_Aim"));
		if (InputActionAimRef.Object)
		{
			AimAction = InputActionAimRef.Object;
		}
	}
}

void ASS_MurdockPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASS_MurdockPlayer::Look);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASS_MurdockPlayer::Sprint);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ASS_MurdockPlayer::Aim);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ASS_MurdockPlayer::Fire);
	EnhancedInputComponent->BindAction(CallAction, ETriggerEvent::Triggered, this, &ASS_MurdockPlayer::Call);
	EnhancedInputComponent->BindAction(CommandAction, ETriggerEvent::Triggered, this, &ASS_MurdockPlayer::ProcessCommandInput);
}

void ASS_MurdockPlayer::ResetPlayerInputVariable()
{
	bCalling = false;
	bReadyForThrowingStrata = false;
	bChangeMontageForThrowingStrata = false;
}

void ASS_MurdockPlayer::BeginPlay()
{
	Super::BeginPlay();

	InitializeStratagem();
	InitializeWeapon();

	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Remove Gun Mesh
	GetMesh()->HideBoneByName(TEXT("gun"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("trap_handle"), EPhysBodyOp::PBO_None);
}

void ASS_MurdockPlayer::SetCharacterControlData(const USSCharacterControlData* CharacterControlData)
{
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SocketOffset = CharacterControlData->RelativeLocation;
}

void ASS_MurdockPlayer::InitializeStratagem()
{
	// Register Stratagem
	USSGameInstance* SSGameInstance = Cast<USSGameInstance>(GetGameInstance());
	USSStratagemManager* StratagemManager = SSGameInstance->GetStratagemManager();
	USSStratagem* DefaultStratagem = StratagemManager->GetStratagem(FName(TEXT("Reinforcements")));
	if (DefaultStratagem)
	{
		AvailableStratagems.Add(std::make_pair(FName(TEXT("Reinforcements")), DefaultStratagem));
	}

	DefaultStratagem = StratagemManager->GetStratagem(FName(TEXT("PrecisionStrike")));
	if (DefaultStratagem)
	{
		AvailableStratagems.Add(std::make_pair(FName(TEXT("PrecisionStrike")), DefaultStratagem));
	}

	DefaultStratagem = StratagemManager->GetStratagem(FName(TEXT("AirStrike")));
	if (DefaultStratagem)
	{
		AvailableStratagems.Add(std::make_pair(FName(TEXT("AirStrike")), DefaultStratagem));
	}
}

void ASS_MurdockPlayer::InitializeWeapon()
{
	MainWeapon = NewObject<USSWeaponComponent_Rifle>(this);
	FName SocketName = MainWeapon->GetTargetSocketName();

	MainWeapon->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		SocketName);

	MainWeapon->RegisterComponent();
}

void ASS_MurdockPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASS_MurdockPlayer::AttemptSprint()
{
	if (!bAiming && bSprintKeyPressing && !GetAnyMontagePlaying(RespawnMontage))
	{
		SetSprintToMovementComponent(true);
	}
}

void ASS_MurdockPlayer::Sprint(const FInputActionValue& Value)
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

void ASS_MurdockPlayer::SetSprintToMovementComponent(bool bNewSprint)
{
	USSCharacterMovementComponent* SSCharacterMovement = Cast<USSCharacterMovementComponent>(GetCharacterMovement());
	SSCharacterMovement->SetSprint(bNewSprint);
}

void ASS_MurdockPlayer::AttemptSprintEndDelegate(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	if (!bAiming && !bCalling && bSprintKeyPressing && GetAnyMontagePlaying(TargetMontage) == false)
	{
		SetSprintToMovementComponent(true);
	}
}

void ASS_MurdockPlayer::Aim(const FInputActionValue& Value)
{
	if (bDead) return;

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

void ASS_MurdockPlayer::SetAimingToMovementComponent(bool bNewAiming)
{
	USSCharacterMovementComponent* SSCharacterMovement = Cast<USSCharacterMovementComponent>(GetCharacterMovement());
	SSCharacterMovement->SetAiming(bNewAiming);
}

void ASS_MurdockPlayer::Fire(const FInputActionValue& Value)
{
	if (bDead) return;

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
		EndDelegate.BindUObject(this, &ASS_MurdockPlayer::AttemptSprintEndDelegate);
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

void ASS_MurdockPlayer::AttackHitCheck(FName AttackId)
{
	Super::AttackHitCheck(AttackId);

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

void ASS_MurdockPlayer::PlaySoundEffect()
{
	MainWeapon->PlaySoundEffect();
}

void ASS_MurdockPlayer::Call(const FInputActionValue& Value)
{
	if (bDead) return;

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
			EndDelegate.BindUObject(this, &ASS_MurdockPlayer::EndCalling);
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

void ASS_MurdockPlayer::EndCalling(UAnimMontage* TargetMontage, bool IsProperlyEnded)
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
		EndDelegate.BindUObject(this, &ASS_MurdockPlayer::EndStrata);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, StrataReadyMontage);
	}
	else
	{
		if (!bAiming && !bCalling && bSprintKeyPressing && GetAnyMontagePlaying(TargetMontage) == false)
		{
			SetSprintToMovementComponent(true);
		}
	}
}

void ASS_MurdockPlayer::EndStrata(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	bReadyForThrowingStrata = false;
}

void ASS_MurdockPlayer::TranslateInput(const FInputActionValue& Value)
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

bool ASS_MurdockPlayer::MatchingInput()
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

void ASS_MurdockPlayer::ProcessCommandInput(const FInputActionValue& Value)
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

void ASS_MurdockPlayer::DetachStrataIndicator()
{
	if (CurStrataIndicator)
	{
		CurStrataIndicator->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurStrataIndicator->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f).Quaternion());
		CurStrataIndicator->SetSimulateCollision();
		CurStrataIndicator->SetStrataCauser(GetController());
	}
}

void ASS_MurdockPlayer::ReleaseThrowable()
{
	// Throw StrataIndicator
	if (CurStrataIndicator)
	{
		DetachStrataIndicator();

		// Throw Camera Direction Vector
		FVector CameraLocation;
		FRotator CameraRotation;
		GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector ThrowPos = CameraLocation + CameraRotation.Vector() * 2000.0f;
		FVector ThrowDirection = ThrowPos - CurStrataIndicator->GetActorLocation();
		ThrowDirection.Normalize();

		CurStrataIndicator->Throw(ThrowDirection);

		CurStrataIndicator = nullptr;
	}
}

void ASS_MurdockPlayer::SetDead()
{
	Super::SetDead();

	DetachStrataIndicator();
	CurStrataIndicator = nullptr;

	USSGameInstance* SSGameInstance = CastChecked<USSGameInstance>(GetGameInstance());
	USSStatisticsManager* SSStatisticsManager = SSGameInstance->GetStatisticsManager();
	SSStatisticsManager->AddDeathCount(GetController(), 1);
	SSStatisticsManager->AddKilledTeammateCount(LastDamageInstigator, 1);
}

float ASS_MurdockPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// DetachStrataIndicator();
	// CurStrataIndicator = nullptr;

	return Result;
}

void ASS_MurdockPlayer::SetupCharacterWidget(USSUserPlayWidget* InUserWidget)
{
	Super::SetupCharacterWidget(InUserWidget);

	if (InUserWidget)
	{
		OnAiming.AddUObject(InUserWidget, &USSUserPlayWidget::UpdateCrossHair);
		OnCalling.AddUObject(InUserWidget, &USSUserPlayWidget::ShowStratagemList);
	}
}

void ASS_MurdockPlayer::Respawn(const FVector& TargetLocation)
{
	Super::Respawn(TargetLocation);

	RpcPlayAnimation(RespawnMontage);
	ClientRpcPlayAnimation(this, RespawnMontage);
}

bool ASS_MurdockPlayer::ServerRpcFire_Validate()
{
	return true;
}

void ASS_MurdockPlayer::ServerRpcFire_Implementation()
{
	FTimerHandle HitCheckTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		HitCheckTimerHandle,
		FTimerDelegate::CreateLambda([&]() {
			AttackHitCheck(TEXT("Fire"));
			}),
		0.07f,
		false);

	RpcPlayAnimation(FireMontage);
}

bool ASS_MurdockPlayer::ServerRpcNotifyFireHit_Validate(const FHitResult& HitResult)
{
	return true;
}

void ASS_MurdockPlayer::ServerRpcNotifyFireHit_Implementation(const FHitResult& HitResult)
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

bool ASS_MurdockPlayer::ServerRpcCalling_Validate()
{
	return true;
}

void ASS_MurdockPlayer::ServerRpcCalling_Implementation()
{
	RpcPlayAnimation(CallMontage);
}

bool ASS_MurdockPlayer::ServerRpcEndCalling_Validate()
{
	return true;
}

void ASS_MurdockPlayer::ServerRpcEndCalling_Implementation()
{
	RpcJumpToSection(CallMontage, TEXT("End"));
}

bool ASS_MurdockPlayer::ServerRpcStrataReady_Validate(const FName& StratagemName)
{
	return true;
}

void ASS_MurdockPlayer::ServerRpcStrataReady_Implementation(const FName& StratagemName)
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

bool ASS_MurdockPlayer::ServerRpcStrataThrow_Validate()
{
	return true;
}

void ASS_MurdockPlayer::ServerRpcStrataThrow_Implementation()
{
	FTimerHandle ThrowTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, 
		this, 
		&ASS_MurdockPlayer::ReleaseThrowable, 
		0.22f, 
		false);

	RpcPlayAnimation(StrataThrowMontage);
}

void ASS_MurdockPlayer::NetMulticastRpcShowAnimationMontage_Implementation(UAnimMontage* MontageToPlay, const float AnimationSpeedRate)
{
	Super::NetMulticastRpcShowAnimationMontage_Implementation(MontageToPlay, AnimationSpeedRate);

	/*if (IsLocallyControlled())
	{
		if (MontageToPlay == HitReactMontage)
		{
			ResetPlayerInputVariable();
			OnCalling.Broadcast(bCalling);
		}
	}*/
}
