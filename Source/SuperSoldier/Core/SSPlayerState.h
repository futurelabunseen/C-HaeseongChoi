// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ASSPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	FORCEINLINE const int32& GetKilledMonsterCount() { return KilledMonsterCount; }
	FORCEINLINE const int32& GetKilledTeammateCount() { return KilledTeammateCount; }
	FORCEINLINE const int32& GetDeathCount() { return DeathCount; }
	FORCEINLINE const int32& GetRevivedTeammateCount() { return RevivedTeammateCount; }
	FORCEINLINE const int32& GetUsedStratagemCount() { return UsedStratagemCount; }

	FORCEINLINE void AddKilledMonsterCount(int32 Count) { KilledMonsterCount = KilledMonsterCount + Count; }
	FORCEINLINE void AddKilledTeammateCount(int32 Count) { KilledTeammateCount = KilledTeammateCount + Count; }
	FORCEINLINE void AddDeathCount(int32 Count) { DeathCount = DeathCount + Count; }
	FORCEINLINE void AddRevivedTeammateCount(int32 Count) { RevivedTeammateCount = RevivedTeammateCount + Count; }
	FORCEINLINE void AddUsedStratagemCount(int32 Count) { UsedStratagemCount = UsedStratagemCount + Count; }
protected:
	UPROPERTY(Replicated)
	int32 KilledMonsterCount;

	UPROPERTY(Replicated)
	int32 KilledTeammateCount;

	UPROPERTY(Replicated)
	int32 DeathCount;

	UPROPERTY(Replicated)
	int32 RevivedTeammateCount;

	UPROPERTY(Replicated)
	int32 UsedStratagemCount;
};
