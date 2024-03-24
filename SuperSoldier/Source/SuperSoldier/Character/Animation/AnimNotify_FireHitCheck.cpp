// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify_FireHitCheck.h"
#include "Interface/SSAnimationAttackInterface.h"

void UAnimNotify_FireHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner())
	{
		ISSAnimationAttackInterface* AttackPawn = Cast<ISSAnimationAttackInterface>(MeshComp->GetOwner());

		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
