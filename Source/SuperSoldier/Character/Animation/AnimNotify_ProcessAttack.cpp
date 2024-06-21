// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify_ProcessAttack.h"
#include "Interface/SSAnimationAttackInterface.h"

void UAnimNotify_ProcessAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* MeshCompOwner = MeshComp->GetOwner();
	if (MeshCompOwner)
	{
		ISSAnimationAttackInterface* AttackPawn = Cast<ISSAnimationAttackInterface>(MeshCompOwner);

		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck(ProcessAttackId);
		}
	}
}
