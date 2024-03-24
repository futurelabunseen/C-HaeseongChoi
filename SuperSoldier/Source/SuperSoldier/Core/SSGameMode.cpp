// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameMode.h"
#include "Core/SSPlayerController.h"
#include "GameFramework/HUD.h"

ASSGameMode::ASSGameMode()
{
	// Set Character
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(
		TEXT("/Script/SuperSoldier.SSMurdockPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	// Set PlayerController
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(
		TEXT("/Script/SuperSoldier.SSPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}
