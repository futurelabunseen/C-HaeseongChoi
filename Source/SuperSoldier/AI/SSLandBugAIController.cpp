// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SSLandBugAIController.h"
#include "BehaviorTree/BehaviorTree.h"

ASSLandBugAIController::ASSLandBugAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(
		TEXT("/Game/SuperSoldier/AI/BT_Bug.BT_Bug"));
	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}
}

void ASSLandBugAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
