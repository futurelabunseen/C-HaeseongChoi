// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Strata/SSStratagem.h"
#include "SSStratPrecisionStrike.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStratPrecisionStrike : public USSStratagem
{
	GENERATED_BODY()
public:
	USSStratPrecisionStrike();
protected:
	virtual void ActivateStratagem(UWorld* const CurWorld, const FVector& TargetLocation);
};
