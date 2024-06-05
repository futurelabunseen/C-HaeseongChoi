// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"
#include "SuperSoldier.h"
#include "UI/SSUserPlayWidget.h"
#include "Character/SSCharacterPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
		UserPlayWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ASSPlayerController::PostInitializeComponents()
{
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::PostInitializeComponents();
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ASSPlayerController::PostNetInit()
{
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::PostNetInit();
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));
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
