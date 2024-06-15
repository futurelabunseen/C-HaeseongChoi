// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SSStatisticsManager.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStatisticsManager : public UObject
{
	GENERATED_BODY()
public:
	USSStatisticsManager();

	void AddKilledMonsterCount(AController* TargetPlayerController, int32 Count);
	void AddKilledTeammateCount(AController* TargetPlayerController, int32 Count);
	void AddDeathCount(AController* TargetPlayerController, int32 Count);
	void AddRevivedTeammateCount(AController* TargetPlayerController, int32 Count);
	void AddUsedStratagemCount(AController* TargetPlayerController, int32 Count);
};
