// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "AIController.h"
#include "Interface/SSCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// Null 체크
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (nullptr == ControllingPawn)
    {
        return EBTNodeResult::Failed;
    }

    ISSCharacterAIInterface* AICharacter = Cast<ISSCharacterAIInterface>(ControllingPawn);
    if (nullptr == AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer")));
    if (nullptr == TargetPawn)
    {
        return EBTNodeResult::Failed;
    }

	// 몬스터에서 타겟으로 향하는 벡터를 구한다.
    FVector ToTargetVec = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
    ToTargetVec.Z = 0.0f;
    ToTargetVec.Normalize();

	// 외적을 통해 오른쪽으로 회전해야하는지 왼쪽으로 회전하야하는지 판별
	// 판별 결과를 인자로 TurnInPlace를 수행하게한다.
    FVector CrossVec = FVector::CrossProduct(ToTargetVec, ControllingPawn->GetActorForwardVector());
    
	bool bTurnRight = CrossVec.Z < 0.0f;
    AICharacter->TurnInPlace(bTurnRight);

	// TurnInPlace가 완료되고 Task가 종료될 수 있게 델리게이트를 설정
	FAICharacterActionFinished OnActionFinished;
	OnActionFinished.BindLambda(
		[&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});

	AICharacter->SetAIActionDelegate(OnActionFinished);

	// 함수 종료 후, FinishLatentTask가 호출되기 전까진 진행 상태로 유지
	return EBTNodeResult::InProgress;
}
