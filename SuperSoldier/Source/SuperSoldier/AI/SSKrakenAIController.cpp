// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SSKrakenAIController.h"
#include "BehaviorTree/BehaviorTree.h"

ASSKrakenAIController::ASSKrakenAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(
		TEXT("/Game/SuperSoldier/AI/BT_Kraken.BT_Kraken"));
	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}
}

void ASSKrakenAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
