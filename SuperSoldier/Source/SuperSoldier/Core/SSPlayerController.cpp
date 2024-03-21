// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerController.h"

void ASSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
