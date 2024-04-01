// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"
#include "SuperSoldier.h"

void ASSPlayerController::BeginPlay()
{
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::BeginPlay();

	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
