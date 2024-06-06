// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify_PlayHitMoanSoundAtLocation.h"
#include "Interface/SSAnimationPlaySoundInterface.h"

void UAnimNotify_PlayHitMoanAtLocation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner())
	{
		ISSAnimationPlaySoundInterface* SoundPlayPawn = Cast<ISSAnimationPlaySoundInterface>(MeshComp->GetOwner());

		if (SoundPlayPawn)
		{
			SoundPlayPawn->PlayMoanSound();
		}
	}
}
