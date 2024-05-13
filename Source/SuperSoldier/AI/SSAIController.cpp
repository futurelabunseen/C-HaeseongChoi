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
}

void ASSAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		RunBehaviorTree(BTAsset);
	}
}

void ASSAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void ASSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}
