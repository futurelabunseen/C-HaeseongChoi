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
                    if (PlayerCharacter->bDead)
                    {
                        continue;
                    }

                    float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
                    if (Distance < ClosestDistance)
                    {
                        ClosestPlayer = PlayerCharacter;
                        ClosestDistance = Distance;
                    }
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
