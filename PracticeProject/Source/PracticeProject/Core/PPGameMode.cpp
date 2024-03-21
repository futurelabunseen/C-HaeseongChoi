// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PPGameMode.h"
#include "PPGameMode.h"

APPGameMode::APPGameMode()
{
	ConstructorHelpers::FClassFinder<APawn> PawnClassRef(
		TEXT("/Script/PracticeProject.PPMurdockPlayer"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}

	PlayerControllerClass;
}
