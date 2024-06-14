// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"
#include "SuperSoldier.h"
#include "UI/SSUserPlayWidget.h"
#include "Character/SSCharacterPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/SSGameState.h"

ASSPlayerController::ASSPlayerController()
{
	// Widget Component
	static ConstructorHelpers::FClassFinder<USSUserPlayWidget> UserPlayWidgetRef(
		TEXT("/Game/SuperSoldier/UI/WBP_PlayWidget.WBP_PlayWidget_C"));
	if (UserPlayWidgetRef.Class)
	{
		UserPlayWidgetClass = UserPlayWidgetRef.Class;
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
		UserPlayWidget->SetVisibility(ESlateVisibility::Hidden);

		ASSGameState* SSGameState = CastChecked<ASSGameState>(GetWorld()->GetGameState());
		SSGameState->OnTotalKilledMonsterCountChangedDelegate.AddUObject(UserPlayWidget, &USSUserPlayWidget::UpdateTotalKillCount);
		UserPlayWidget->UpdateTotalKillCount(SSGameState->GetTotalKilledMonsterCount());
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

void ASSPlayerController::ClientRpcBlendViewTargetToNewPawn_Implementation(APawn* NewPawn)
{
	SetViewTargetWithBlend(NewPawn, 0.8f);
}
