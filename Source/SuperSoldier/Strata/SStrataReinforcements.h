// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Strata/SSStratagem.h"
#include "SStrataReinforcements.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USStrataReinforcements : public USSStratagem
{
	GENERATED_BODY()
public:
	USStrataReinforcements();
	virtual void ActivateStratagem(UWorld* const CurWorld, const FVector& TargetLocation);
};
