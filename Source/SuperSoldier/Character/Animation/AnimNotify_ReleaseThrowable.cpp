// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify_ReleaseThrowable.h"
#include "Character/SSCharacterBase.h"

void UAnimNotify_ReleaseThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner())
	{
		ASSCharacterBase* ThrowableChracter = Cast<ASSCharacterBase>(MeshComp->GetOwner());

		if (ThrowableChracter)
		{
			ThrowableChracter->ReleaseThrowable();
		}
	}
}
