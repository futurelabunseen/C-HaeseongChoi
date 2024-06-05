// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterPlayer.h"
#include "SuperSoldier.h"
#include "EngineUtils.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CharacterStat/SSCharacterStatComponent.h"

ASSCharacterPlayer::ASSCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Input Action & Input Mapping Context
	{
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

void ASSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASSCharacterPlayer::Move);
}

void ASSCharacterPlayer::Respawn(const FVector& TargetLocation)
{
	bDead = false;
	Stat->Initialize();
	OnRep_ServerCharacterbDead();
}

void ASSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// If Locally Controlled
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(NormalInputMappingContext, 0);
		}
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
				AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
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

void ASSCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if (IsLocallyControlled())
	//{
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);                // (1,0,0) 카메라 전방
	//	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);                    // (0,1,0) 

	//	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForwardDirection * 100.0f, 10.0f, FColor::Cyan, false, -1, 0, 5.0f);
	//	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + RightDirection * 100.0f, 10.0f, FColor::Blue, false, -1, 0, 5.0f);

	//	FVector Start = GetActorUpVector() * 10.0f + GetActorLocation();
	//	FVector End = GetActorForwardVector() * 50.0f + Start;
	//	DrawDebugDirectionalArrow(GetWorld(), Start, End, 10.0f, FColor::Red, false, -1, 0, 10.0f);
	//}
}
