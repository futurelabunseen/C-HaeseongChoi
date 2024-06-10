// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SStrataReinforcements.h"
#include "Character/SSCharacterPlayer.h"
#include "Character/SS_RespawnTankPlayer.h"
#include "Core/SSPlayerController.h"
#include "EngineUtils.h"
#include "Core/SSGameMode.h"

USStrataReinforcements::USStrataReinforcements()
{
	DelayTime = 5.0f;
	CoolTime = 20.0f;
	StrataType = EStrataType::SUPPORT;

	CommandArray = TArray<EStrataCommand>{ 
		EStrataCommand::UP, 
		EStrataCommand::DOWN, 
		EStrataCommand::RIGHT, 
		EStrataCommand::LEFT,
		EStrataCommand::UP };
}

void USStrataReinforcements::ActivateStratagem(UWorld* const CurWorld, const FVector& TargetLocation)
{
	// Revive All Dead Player
	if (!IsValid(CurWorld)) return;

	for (APlayerController* PlayerController : TActorRange<APlayerController>(CurWorld))
	{
		ASSCharacterPlayer* PlayerCharacter = CastChecked<ASSCharacterPlayer>(PlayerController->GetCharacter());
		if (PlayerCharacter->bDead)
		{
			ASSGameMode* SSGameMode = CastChecked<ASSGameMode>(CurWorld->GetAuthGameMode());
			SSGameMode->RespawnPlayers(TargetLocation);
		}
	}
}
