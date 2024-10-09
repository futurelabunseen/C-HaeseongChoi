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

	// Null 체크
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

	// 타겟 플레이어까지의 거리가 공격 범위보다 작으면 성공
    float DistanceToTarget = ControllingPawn->GetDistanceTo(TargetPawn);
    const float AttackRange = AICharacter->GetAttackRange();
	bResult = (DistanceToTarget <= AttackRange);
    
	return bResult;
}
