// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameInstance.h"
#include "Strata/SSStratagemManager.h"

USSGameInstance::USSGameInstance()
{
	StratagemManager = CreateDefaultSubobject<USSStratagemManager>(TEXT("StratagemManager"));
}

void USSGameInstance::Init()
{
	Super::Init();

	StratagemManager->InitializeStratagem();
}
