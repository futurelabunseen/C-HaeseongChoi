// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Strata/SSStratagem.h"
#include "SSStrataAirStrike.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStrataAirStrike : public USSStratagem
{
	GENERATED_BODY()
public:
	USSStrataAirStrike();
	virtual void ActivateStratagem(UWorld* const CurWorld, AActor* const StrataCauser, const FVector& TargetLocation);
};
