// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"
#include "SuperSoldier.h"
#include "UI/SSUserPlayWidget.h"
#include "UI/SSUserResultWidget.h"
#include "Character/SSCharacterPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/SSGameState.h"
#include "Core/SSPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Core/SSGameMode.h"

ASSPlayerController::ASSPlayerController()
{
	// Widget Component
	static ConstructorHelpers::FClassFinder<USSUserPlayWidget> UserPlayWidgetRef(
		TEXT("/Game/SuperSoldier/UI/BP/WBP_PlayWidget.WBP_PlayWidget_C"));
	if (UserPlayWidgetRef.Class)
	{
		UserPlayWidgetClass = UserPlayWidgetRef.Class;
	}

	static ConstructorHelpers::FClassFinder<USSUserResultWidget> UserResultWidgetRef(
		TEXT("/Game/SuperSoldier/UI/BP/WBP_UserStatistics.WBP_UserStatistics_C"));
	if (UserResultWidgetRef.Class)
	{
		UserResultWidgetClass = UserResultWidgetRef.Class;
	}

	// Spectate Input Action & Input Mapping Context
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(
			TEXT("/Game/SuperSoldier/Input/IMC_Spectate.IMC_Spectate"));
		if (InputMappingContextRef.Object)
		{
			SpectateInputMappingContext = InputMappingContextRef.Object;
		}

		static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSpectateNextRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_SpectateNext.IA_SpectateNext"));
		if (InputActionSpectateNextRef.Object)
		{
			SpectateNextAction = InputActionSpectateNextRef.Object;
		}

		static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSpectatePreviousRef(
			TEXT("/Game/SuperSoldier/Input/Actions/IA_SpectatePrevious.IA_SpectatePrevious"));
		if (InputActionSpectatePreviousRef.Object)
		{
			SpectatePreviousAction = InputActionSpectatePreviousRef.Object;
		}
	}
}

void ASSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		FInputModeGameOnly GameOnlyInputMode;
		SetInputMode(GameOnlyInputMode);

		UserPlayWidget = CreateWidget<USSUserPlayWidget>(this, UserPlayWidgetClass);
		UserPlayWidget->AddToViewport();
		UserPlayWidget->SetVisibility(ESlateVisibility::Visible);

		ASSGameState* SSGameState = CastChecked<ASSGameState>(GetWorld()->GetGameState());
		SSGameState->OnTotalKilledMonsterCountChangedDelegate.AddUObject(UserPlayWidget, &USSUserPlayWidget::UpdateTotalKillCount);
		SSGameState->OnRemainPlayerRespawnCountChangedDelegate.AddUObject(UserPlayWidget, &USSUserPlayWidget::UpdateRemainPlayerSpawnCount);
		UserPlayWidget->UpdateTotalKillCount(SSGameState->GetTotalKilledMonsterCount());
		UserPlayWidget->FadeInOut(true);
	}
}

void ASSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASSPlayerController::PostNetInit()
{
	Super::PostNetInit();
}

void ASSPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	OnPossessionChanged.Broadcast(aPawn);
}

void ASSPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	ASSCharacterPlayer* SSCharacterPlayer = CastChecked<ASSCharacterPlayer>(P);
	SSCharacterPlayer->SetupCharacterWidget(UserPlayWidget);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();

		UInputMappingContext* MappingContext = SSCharacterPlayer->GetIMC();
		if (IsValid(SpectateInputMappingContext))
		{
			Subsystem->AddMappingContext(SpectateInputMappingContext, 0);
		}

		if (IsValid(MappingContext))
		{
			Subsystem->AddMappingContext(SSCharacterPlayer->GetIMC(), 0);
		}
	}
}

void ASSPlayerController::ShowGameResult(bool bCleared)
{
	ASSPlayerState* SSPlayerState = GetPlayerState<ASSPlayerState>();
	UserResultWidget = CreateWidget<USSUserResultWidget>(this, UserResultWidgetClass);
	
	UserResultWidget->AddToViewport();
	UserResultWidget->SetVisibility(ESlateVisibility::Visible);

	UserResultWidget->UpdateKillCount(SSPlayerState->GetKilledMonsterCount());
	UserResultWidget->UpdateTeamKillCount(SSPlayerState->GetKilledTeammateCount());
	UserResultWidget->UpdateDeathCount(SSPlayerState->GetDeathCount());
	UserResultWidget->UpdateReviveCount(SSPlayerState->GetRevivedTeammateCount());
	UserResultWidget->UpdateUsedStratagemNum(SSPlayerState->GetUsedStratagemCount());
	UserResultWidget->UpdateGameResultText(bCleared);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
	}

	bShowMouseCursor = true;
}

void ASSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	SS_LOG(LogSSNetwork, Log, TEXT("SetupInputComponent"));

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(SpectateNextAction, ETriggerEvent::Triggered, this, &ASSPlayerController::SpectateNext);
	EnhancedInputComponent->BindAction(SpectatePreviousAction, ETriggerEvent::Triggered, this, &ASSPlayerController::SpectatePrevious);
}

void ASSPlayerController::UpdateViewTarget(APawn* aPawn)
{
	if(IsValid(aPawn))
	{
		SetViewTargetWithBlend(aPawn);
	}
	else
	{
		SpectateTarget();
	}
}

void ASSPlayerController::ServerRpcSpectatePrevious_Implementation()
{
	ASSCharacterPlayer* SSCharacterPlayer = CastChecked<ASSCharacterPlayer>(GetCharacter());
	if (!SSCharacterPlayer->bDead) return;

	UpdatePlayerIndex();
	ClearPrevDelegate();

	SpectateOffset -= 1;
	
	SpectateTarget();
}

void ASSPlayerController::ServerRpcSpectateNext_Implementation()
{
	ASSCharacterPlayer* SSCharacterPlayer = CastChecked<ASSCharacterPlayer>(GetCharacter());
	if (!SSCharacterPlayer->bDead) return;

	UpdatePlayerIndex();
	ClearPrevDelegate();

	SpectateOffset += 1;

	SpectateTarget();
}

void ASSPlayerController::SpectatePrevious(const FInputActionValue& Value)
{
	ASSCharacterPlayer* SSCharacterPlayer = CastChecked<ASSCharacterPlayer>(GetCharacter());
	if (!SSCharacterPlayer->bDead) return;

	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		ServerRpcSpectatePrevious();
	}
}

void ASSPlayerController::SpectateNext(const FInputActionValue& Value)
{
	ASSCharacterPlayer* SSCharacterPlayer = CastChecked<ASSCharacterPlayer>(GetCharacter());
	if (!SSCharacterPlayer->bDead) return;

	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		ServerRpcSpectateNext();
	}
}

void ASSPlayerController::ClearPrevDelegate()
{
	// 기존 델리게이트를 삭제
	ASSPlayerController* CurrentViewTargetPlayerController =
		Cast<ASSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), IndividualPlayerIndex + SpectateOffset));
	if (IsValid(CurrentViewTargetPlayerController))
	{
		CurrentViewTargetPlayerController->OnPossessedPawnChanged.RemoveAll(this);
	}
}

void ASSPlayerController::UpdatePlayerIndex()
{
	ASSGameMode* SSGameMode = CastChecked<ASSGameMode>(GetWorld()->GetAuthGameMode());
	IndividualPlayerIndex = SSGameMode->GetPlayerIndex(this);
}

void ASSPlayerController::SpectateTarget()
{
	ASSGameMode* SSGameMode = CastChecked<ASSGameMode>(GetWorld()->GetAuthGameMode());
	int32 PlayerIndex = SSGameMode->GetPlayerIndex(this);

	int32 ClampedIndex = SSGameMode->ClampPlayerIndex(PlayerIndex + SpectateOffset);
	SpectateOffset = ClampedIndex - PlayerIndex;

	ASSPlayerController* SpectateTargetPlayerController =
		Cast<ASSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), ClampedIndex));
	ASSCharacterPlayer* SpectateTarget = Cast<ASSCharacterPlayer>(SpectateTargetPlayerController->GetCharacter());
	SpectateTarget->SetupCharacterWidget(UserPlayWidget);
	SetViewTargetWithBlend(SpectateTarget);

	// 새 델리게이트를 추가
	SpectateTargetPlayerController->OnPossessionChanged.AddUObject(this, &ASSPlayerController::UpdateViewTarget);
}

void ASSPlayerController::ClientRpcSetAndShowFinalGameStatistics_Implementation(bool bCleared, const FPlayStatistics& FinalPlayStatistics)
{
	ASSPlayerState* SSPlayerState = GetPlayerState<ASSPlayerState>();
	SSPlayerState->SetPlayStatistics(FinalPlayStatistics);
	ShowGameResult(bCleared);
}
