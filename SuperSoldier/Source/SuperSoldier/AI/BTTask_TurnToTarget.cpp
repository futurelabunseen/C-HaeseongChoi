// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "AIController.h"
#include "Character/SSCharacterNonPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer")));
    if (nullptr == TargetPawn)
    {
        return EBTNodeResult::Failed;
    }

    FVector ToTargetVec = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
    ToTargetVec.Z = 0.0f;

    FRotator ToTargetRot = FRotationMatrix::MakeFromX(ToTargetVec).Rotator();
    double YawDelta = ToTargetRot.Yaw - ControllingPawn->GetActorRotation().Yaw;
    bool bTurnRight = YawDelta > 0.0f;

    FAICharacterActionFinished OnActionFinished;
    OnActionFinished.BindLambda(
        [&]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        });

    CharacterNonPlayer->SetAIActionDelegate(OnActionFinished);
    CharacterNonPlayer->TurnInPlace(bTurnRight);

	return EBTNodeResult::InProgress;
}
