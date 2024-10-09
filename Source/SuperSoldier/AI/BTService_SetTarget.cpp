// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetTarget.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SSCharacterPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SetTarget::UBTService_SetTarget()
{
	NodeName = TEXT("SetTarget");
	Interval = 1.0f;
}

void UBTService_SetTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

    if (ControllingPawn)
    {
		// 플레이어 캐릭터 클래스로 생성된 모든 액터를 가져온다.
        TArray<AActor*> Actors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSCharacterPlayer::StaticClass(), Actors);

        if (!Actors.IsEmpty())
        {
            ASSCharacterPlayer* ClosestPlayer = nullptr;
            float ClosestDistance = MAX_FLT;
            for (AActor* Actor : Actors)
            {
                if (ASSCharacterPlayer* PlayerCharacter = Cast<ASSCharacterPlayer>(Actor))
                {
					// 죽어있는 플레이어는 타게팅하지 않는다.
                    if (PlayerCharacter->bDead)
                    {
                        continue;
                    }
					
					// 가장 가까운 플레이어를 찾는다.
                    float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
                    if (Distance < ClosestDistance)
                    {
                        ClosestPlayer = PlayerCharacter;
                        ClosestDistance = Distance;
                    }
                }
            }

			// 가장 가까운 플레이어를 BlackBoard Data의 TargetPlayer에 설정한다.
            UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), ClosestPlayer);
            }
        }
    }
}
