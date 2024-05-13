// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SSAIController.h"
#include "SSKrakenAIController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSKrakenAIController : public ASSAIController
{
	GENERATED_BODY()
public:
	ASSKrakenAIController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
};
