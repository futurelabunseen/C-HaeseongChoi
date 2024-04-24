// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_AttackInRange.h"
#include "AIController.h"
#include "Character/SSCharacterNonPlayer.h"
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

    ASSCharacterNonPlayer* CharacterNonPlayer = Cast<ASSCharacterNonPlayer>(ControllingPawn);
    if (nullptr == CharacterNonPlayer)
    {
        return false;
    }

    APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer")));
    if (nullptr == TargetPawn)
    {
        return false;
    }

    float DistanceToTarget = ControllingPawn->GetDistanceTo(TargetPawn);
    const float AttackRange = 620.0f;
    bResult = (DistanceToTarget <= AttackRange);
    
	return bResult;
}
