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
		if (MappingContext)
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

void ASSPlayerController::ClientRpcSetAndShowFinalGameStatistics_Implementation(bool bCleared, const FPlayStatistics& FinalPlayStatistics)
{
	ASSPlayerState* SSPlayerState = GetPlayerState<ASSPlayerState>();
	SSPlayerState->SetPlayStatistics(FinalPlayStatistics);
	ShowGameResult(bCleared);
}

void ASSPlayerController::ClientRpcBlendViewTargetToNewPawn_Implementation(APawn* NewPawn)
{
	SetViewTargetWithBlend(NewPawn, 0.8f);
}
