// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"
#include "SuperSoldier.h"
#include "UI/SSUserPlayWidget.h"
#include "Character/SSCharacterBase.h"

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
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::BeginPlay();

	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	if (IsLocalController())
	{
		UserPlayWidget = CreateWidget<USSUserPlayWidget>(this, UserPlayWidgetClass);
		UserPlayWidget->AddToViewport();
		UserPlayWidget->SetVisibility(ESlateVisibility::Visible);

		ASSCharacterBase* SSCharacter = Cast<ASSCharacterBase>(GetCharacter());
		if (SSCharacter)
		{
			SSCharacter->SetupCharacterWidget(UserPlayWidget);
		}
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
