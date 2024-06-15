// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SSGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTotalKilledMonsterCountChangedDelegate, int32);

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// InGame KilledMonsterCount Section
public:
	FORCEINLINE const int32& GetTotalKilledMonsterCount() { return TotalKilledMonsterCount; }
	FORCEINLINE void SetTotalKilledMonsterCount(int32 NewTotalKilledMonsterCount) { TotalKilledMonsterCount = NewTotalKilledMonsterCount; }

	FOnTotalKilledMonsterCountChangedDelegate OnTotalKilledMonsterCountChangedDelegate;
protected:
	UFUNCTION()
	virtual void OnRep_TotalKilledMonsterCount();

	UPROPERTY(ReplicatedUsing = OnRep_TotalKilledMonsterCount)
	int32 TotalKilledMonsterCount = 0;
};
