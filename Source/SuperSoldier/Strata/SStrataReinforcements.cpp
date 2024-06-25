// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SStrataReinforcements.h"
#include "Character/SSCharacterPlayer.h"
#include "Character/SS_RespawnTankPlayer.h"
#include "Core/SSPlayerController.h"
#include "EngineUtils.h"
#include "Core/SSGameMode.h"
#include "Core/SSGameInstance.h"
#include "Core/SSStatisticsManager.h"

USStrataReinforcements::USStrataReinforcements()
{
	DelayTime = 5.0f;
	CoolTime = 0.0f;
	StrataType = EStrataType::SUPPORT;

	CommandArray = TArray<EStrataCommand>{ 
		EStrataCommand::UP, 
		EStrataCommand::DOWN, 
		EStrataCommand::RIGHT, 
		EStrataCommand::LEFT,
		EStrataCommand::UP };
}

void USStrataReinforcements::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector& TargetLocation)
{
	Super::ActivateStratagem(CurWorld, StrataCauser, TargetLocation);

	if (!IsValid(CurWorld)) return;

	// 죽은 플레이어를 모두 되살린다.
	ASSGameMode* SSGameMode = CastChecked<ASSGameMode>(CurWorld->GetAuthGameMode());
	int32 RespawnedPlayerNum = SSGameMode->RespawnPlayers(TargetLocation);

	USSGameInstance* SSGameInstance = CastChecked<USSGameInstance>(CurWorld->GetGameInstance());
	USSStatisticsManager* SSStatisticsManager = SSGameInstance->GetStatisticsManager();
	SSStatisticsManager->AddRevivedTeammateCount(StrataCauser, RespawnedPlayerNum);
}
