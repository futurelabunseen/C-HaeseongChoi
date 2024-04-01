// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameState.h"
#include "SuperSoldier.h"

void ASSGameState::HandleBeginPlay()
{
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::HandleBeginPlay();
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ASSGameState::OnRep_ReplicatedHasBegunPlay()
{
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::OnRep_ReplicatedHasBegunPlay();
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));
}