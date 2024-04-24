// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SSAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

ASSAIController::ASSAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardDataRef(
		TEXT("/Game/SuperSoldier/AI/BB_Enemy.BB_Enemy"));
	if (BlackboardDataRef.Object)
	{
		BBAsset = BlackboardDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(
		TEXT("/Game/SuperSoldier/AI/BT_Enemy.BT_Enemy"));
	if (BehaviorTreeRef.Object)
	{
		BTAsset = BehaviorTreeRef.Object;
	}
}

void ASSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		RunBehaviorTree(BTAsset);
	}
}
