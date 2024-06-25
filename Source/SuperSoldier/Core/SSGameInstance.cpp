// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameInstance.h"
#include "Strata/SSStratagemManager.h"

USSGameInstance::USSGameInstance()
{
	StratagemManager = CreateDefaultSubobject<USSStratagemManager>(TEXT("StratagemManager"));
	DeadSoundConcurrency = CreateDefaultSubobject<USoundConcurrency>(TEXT("DeadSoundConcurrency"));
	DeadSoundConcurrency->Concurrency.MaxCount = 4;
	DeadSoundConcurrency->Concurrency.ResolutionRule = EMaxConcurrentResolutionRule::StopFarthestThenOldest;
}

void USSGameInstance::Init()
{
	Super::Init();
	StratagemManager->InitializeStratagem();
}
