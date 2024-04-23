// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SSAIController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSAIController : public AAIController
{
	GENERATED_BODY()
public:
	ASSAIController();
protected:
	virtual void OnPossess(APawn* InPawn) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBehaviorTree> BTAsset;
};
