// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetTarget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API UBTService_SetTarget : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_SetTarget();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
