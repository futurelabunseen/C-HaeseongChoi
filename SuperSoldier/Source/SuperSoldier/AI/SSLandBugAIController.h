// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SSAIController.h"
#include "SSLandBugAIController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSLandBugAIController : public ASSAIController
{
	GENERATED_BODY()
public:
	ASSLandBugAIController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
};
