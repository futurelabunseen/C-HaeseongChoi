// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "Character/SSCharacterNonPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
	// Null 체크
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

	// 캐릭터의 공격이 끝났을 때, Task를 완료
    FAICharacterActionFinished OnActionFinished;
    OnActionFinished.BindLambda(
        [&]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        });

	// 몬스터의 공격 모션 중, 한가지를 선택
	TArray<FName> AttackMontageSectionNames = CharacterNonPlayer->GetAttackMontageSectionNames();
	int RandomMontageSectionNameIndex = FMath::RandRange(0, AttackMontageSectionNames.Num() - 1);

	check(AttackMontageSectionNames.IsValidIndex(RandomMontageSectionNameIndex));

	// 선택한 공격 모션으로 공격
    CharacterNonPlayer->SetAIActionDelegate(OnActionFinished);
    CharacterNonPlayer->Attack(AttackMontageSectionNames[RandomMontageSectionNameIndex]);

	// 함수 종료 후, FinishLatentTask가 호출되기 전까진 진행 상태로 유지
    return EBTNodeResult::InProgress;
}
