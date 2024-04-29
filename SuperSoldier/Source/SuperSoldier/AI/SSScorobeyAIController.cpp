// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SSScorobeyAIController.h"
#include "BehaviorTree/BehaviorTree.h"

ASSScorobeyAIController::ASSScorobeyAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(
		TEXT("/Game/SuperSoldier/AI/BT_Bug.BT_Bug"));
	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}
}

void ASSScorobeyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
