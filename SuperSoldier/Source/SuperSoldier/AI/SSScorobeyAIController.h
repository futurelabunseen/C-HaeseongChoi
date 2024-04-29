// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SSAIController.h"
#include "SSScorobeyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSScorobeyAIController : public ASSAIController
{
	GENERATED_BODY()
public:
	ASSScorobeyAIController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
};
