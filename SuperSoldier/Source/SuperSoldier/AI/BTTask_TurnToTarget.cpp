// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "AIController.h"
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

    APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer")));
    if (nullptr == TargetPawn)
    {
        return EBTNodeResult::Failed;
    }

    const float TurnSpeed = 2.0f;
    FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
    LookVector.Z = 0.0f;

    FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    ControllingPawn->SetActorRotation(
        FMath::RInterpTo(
            ControllingPawn->GetActorRotation(),
            TargetRot,
            GetWorld()->GetDeltaSeconds(),
            TurnSpeed));

	return EBTNodeResult::Succeeded;
}
