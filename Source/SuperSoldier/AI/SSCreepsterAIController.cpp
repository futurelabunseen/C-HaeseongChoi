// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SSCreepsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"

ASSCreepsterAIController::ASSCreepsterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(
		TEXT("/Game/SuperSoldier/AI/BT_Bug.BT_Bug"));
	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}
}

void ASSCreepsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
