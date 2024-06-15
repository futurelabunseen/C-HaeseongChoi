// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSStatisticsManager.h"
#include "Core/SSPlayerState.h"

USSStatisticsManager::USSStatisticsManager()
{
}

void USSStatisticsManager::AddKilledMonsterCount(AController* TargetPlayerController, int32 Count)
{
	ASSPlayerState* SSPlayerState = TargetPlayerController->GetPlayerState<ASSPlayerState>();

	if (SSPlayerState)
	{
		SSPlayerState->AddKilledMonsterCount(Count);
	}
}

void USSStatisticsManager::AddKilledTeammateCount(AController* TargetPlayerController, int32 Count)
{
	ASSPlayerState* SSPlayerState = TargetPlayerController->GetPlayerState<ASSPlayerState>();

	if (SSPlayerState)
	{
		SSPlayerState->AddKilledTeammateCount(Count);
	}
}

void USSStatisticsManager::AddDeathCount(AController* TargetPlayerController, int32 Count)
{
	ASSPlayerState* SSPlayerState = TargetPlayerController->GetPlayerState<ASSPlayerState>();

	if (SSPlayerState)
	{
		SSPlayerState->AddDeathCount(Count);
	}
}

void USSStatisticsManager::AddRevivedTeammateCount(AController* TargetPlayerController, int32 Count)
{
	ASSPlayerState* SSPlayerState = TargetPlayerController->GetPlayerState<ASSPlayerState>();

	if (SSPlayerState)
	{
		SSPlayerState->AddRevivedTeammateCount(Count);
	}
}

void USSStatisticsManager::AddUsedStratagemCount(AController* TargetPlayerController, int32 Count)
{
	ASSPlayerState* SSPlayerState = TargetPlayerController->GetPlayerState<ASSPlayerState>();

	if (SSPlayerState)
	{
		SSPlayerState->AddUsedStratagemCount(Count);
	}
}
