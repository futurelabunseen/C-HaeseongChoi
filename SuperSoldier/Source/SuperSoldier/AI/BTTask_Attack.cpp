// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "Character/SSCharacterNonPlayer.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (nullptr == ControllingPawn)
    {
        return EBTNodeResult::Failed;
    }

    ASSCharacterNonPlayer* CharacterNonPlayer = Cast<ASSCharacterNonPlayer>(ControllingPawn);
    if (nullptr == CharacterNonPlayer)
    {
        return EBTNodeResult::Failed;
    }

    FAICharacterAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        });

    CharacterNonPlayer->SetAIAttackDelegate(OnAttackFinished);
    CharacterNonPlayer->Attack();

    return EBTNodeResult::InProgress;
}
