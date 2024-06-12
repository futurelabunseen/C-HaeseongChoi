// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_AttackInRange.h"
#include "AIController.h"
#include "Interface/SSCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (nullptr == ControllingPawn)
    {
        return false;
    }

    ISSCharacterAIInterface* AICharacter = Cast<ISSCharacterAIInterface>(ControllingPawn);
    if (nullptr == AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer")));
    if (nullptr == TargetPawn)
    {
        return false;
    }

    float DistanceToTarget = ControllingPawn->GetDistanceTo(TargetPawn);
    const float AttackRange = AICharacter->GetAttackRange();
	bResult = (DistanceToTarget <= AttackRange);
    
	return bResult;
}
