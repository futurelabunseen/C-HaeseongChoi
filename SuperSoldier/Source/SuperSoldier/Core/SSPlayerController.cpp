// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"
#include "SuperSoldier.h"

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

void ASSPlayerController::BeginPlay()
{
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::BeginPlay();

	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
