// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SSAIController.h"
#include "SSCreepsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSCreepsterAIController : public ASSAIController
{
	GENERATED_BODY()
public:
	ASSCreepsterAIController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
};
