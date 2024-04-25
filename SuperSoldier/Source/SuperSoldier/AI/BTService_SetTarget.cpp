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
        TArray<AActor*> PlayerCharacters;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSCharacterPlayer::StaticClass(), PlayerCharacters);

        if (!PlayerCharacters.IsEmpty())
        {
            AActor* ClosestPlayer = nullptr;
            float ClosestDistance = MAX_FLT;
            for (AActor* Player : PlayerCharacters)
            {
                float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), Player->GetActorLocation());
                if (Distance < ClosestDistance)
                {
                    ClosestPlayer = Player;
                    ClosestDistance = Distance;
                }
            }

            UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), ClosestPlayer);
            }
        }
    }
}
