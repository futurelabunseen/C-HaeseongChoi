// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterPlayer.h"
#include "SuperSoldier.h"
#include "EngineUtils.h"
#include "Character/CharacterStat/SSCharacterStatComponent.h"

ASSCharacterPlayer::ASSCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool ASSCharacterPlayer::GetAnyMontagePlaying(UAnimMontage* FilterMontage)
{
	bool bRet = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (FilterMontage == NULL)
	{
		bRet = AnimInstance->IsAnyMontagePlaying();
	}
	else
	{
		UAnimMontage* CurrentActivateMontage = AnimInstance->GetCurrentActiveMontage();
		UClass* CurMontageClass = CurrentActivateMontage->StaticClass();
		UClass* FilterMontageClass = FilterMontage->StaticClass();

		bRet = AnimInstance->IsAnyMontagePlaying() &&
			CurMontageClass != FilterMontageClass;
	}
	return bRet;
}

void ASSCharacterPlayer::Respawn(const FVector& TargetLocation)
{
	bDead = false;
	Stat->Initialize();

	SetActorLocation(TargetLocation);

	OnRep_ServerCharacterbDead();
}

void ASSCharacterPlayer::RpcPlayAnimation(UAnimMontage* MontageToPlay)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				ASSCharacterPlayer* OtherPlayer = Cast<ASSCharacterPlayer>(PlayerController->GetPawn());

				if (OtherPlayer)
				{
					OtherPlayer->ClientRpcPlayAnimation(this, MontageToPlay);
				}
			}
		}
	}
}

void ASSCharacterPlayer::RpcJumpToSection(UAnimMontage* MontageToPlay, FName SectionName)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				ASSCharacterPlayer* OtherPlayer = Cast<ASSCharacterPlayer>(PlayerController->GetPawn());

				if (OtherPlayer)
				{
					OtherPlayer->ClientRpcJumpToSection(this, MontageToPlay, SectionName);
				}
			}
		}
	}
}

void ASSCharacterPlayer::ClientRpcJumpToSection_Implementation(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay, FName SectionName)
{
	if (CharacterToPlay)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = CharacterToPlay->GetMesh()->GetAnimInstance();
		FName CurSection = AnimInstance->Montage_GetCurrentSection(MontageToPlay);

		if (CurSection != NAME_None)
		{
			bool bNotAlreadyPlaying = CurSection.Compare(SectionName) != 0;

			if (bNotAlreadyPlaying)
			{
				AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
			}
		}
	}
}

void ASSCharacterPlayer::ClientRpcPlayAnimation_Implementation(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay)
{
	if (CharacterToPlay)
	{
		const float AnimationSpeedRate = 1.0f;
		UAnimInstance* AnimInstance = CharacterToPlay->GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MontageToPlay, AnimationSpeedRate);
	}
}

void ASSCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
