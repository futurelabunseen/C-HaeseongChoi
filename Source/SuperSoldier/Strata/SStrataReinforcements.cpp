// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SStrataReinforcements.h"
#include "Character/SSCharacterPlayer.h"
#include "Character/SS_RespawnTankPlayer.h"

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

	for (FConstPlayerControllerIterator It = CurWorld->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		ASSCharacterPlayer* PlayerCharacter = Cast<ASSCharacterPlayer>(PlayerController->GetCharacter());
		check(PlayerCharacter);

		if (PlayerCharacter->bDead)
		{
			FVector RespawnLocation = TargetLocation;
			RespawnLocation.Z += 5000.0f;

			ASS_RespawnTankPlayer* RespawnTank = CurWorld->SpawnActor<ASS_RespawnTankPlayer>(ASS_RespawnTankPlayer::StaticClass());
			RespawnTank->SetActorLocation(RespawnLocation);
			PlayerController->Possess(RespawnTank);

			PlayerCharacter->SetActorHiddenInGame(true);
			PlayerCharacter->SetLifeSpan(2.0f);
		}
	}
}
