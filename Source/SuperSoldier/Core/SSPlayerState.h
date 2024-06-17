// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SSPlayerState.generated.h"

USTRUCT()
struct FPlayStatistics
{
	GENERATED_BODY()

	UPROPERTY()
	int32 KilledMonsterCount = 0;

	UPROPERTY()
	int32 KilledTeammateCount = 0;

	UPROPERTY()
	int32 DeathCount = 0;

	UPROPERTY()
	int32 RevivedTeammateCount = 0;

	UPROPERTY()
	int32 UsedStratagemCount = 0;
};

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ASSPlayerState();

	FORCEINLINE const int32& GetKilledMonsterCount() { return PlayStatistics.KilledMonsterCount; }
	FORCEINLINE const int32& GetKilledTeammateCount() { return PlayStatistics.KilledTeammateCount; }
	FORCEINLINE const int32& GetDeathCount() { return PlayStatistics.DeathCount; }
	FORCEINLINE const int32& GetRevivedTeammateCount() { return PlayStatistics.RevivedTeammateCount; }
	FORCEINLINE const int32& GetUsedStratagemCount() { return PlayStatistics.UsedStratagemCount; }

	FORCEINLINE void AddKilledMonsterCount(int32 Count) { PlayStatistics.KilledMonsterCount = PlayStatistics.KilledMonsterCount + Count; }
	FORCEINLINE void AddKilledTeammateCount(int32 Count) { PlayStatistics.KilledTeammateCount = PlayStatistics.KilledTeammateCount + Count; }
	FORCEINLINE void AddDeathCount(int32 Count) { PlayStatistics.DeathCount = PlayStatistics.DeathCount + Count; }
	FORCEINLINE void AddRevivedTeammateCount(int32 Count) { PlayStatistics.RevivedTeammateCount = PlayStatistics.RevivedTeammateCount + Count; }
	FORCEINLINE void AddUsedStratagemCount(int32 Count) { PlayStatistics.UsedStratagemCount = PlayStatistics.UsedStratagemCount + Count; }

	FORCEINLINE void SetPlayStatistics(const FPlayStatistics& NewPlayStatistics) { PlayStatistics = NewPlayStatistics; }
	FORCEINLINE const FPlayStatistics& GetPlayStatistics() { return PlayStatistics; }
protected:
	FPlayStatistics PlayStatistics;
};
