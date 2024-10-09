// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStratagem.h"
#include "Core/SSGameInstance.h"
#include "Core/SSStatisticsManager.h"

USSStratagem::USSStratagem()
{
	DelayTime = 0.0f;
}

const TArray<EStrataCommand>& USSStratagem::GetCommandSequence() const
{
	return CommandArray;
}

void USSStratagem::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector TargetLocation, const FVector ThrowedDirection)
{
	if (!IsValid(CurWorld)) return;

	USSGameInstance* SSGameInstance = CastChecked<USSGameInstance>(CurWorld->GetGameInstance());
	USSStatisticsManager* SSStatisticsManager = SSGameInstance->GetStatisticsManager();
	SSStatisticsManager->AddUsedStratagemCount(StrataCauser, 1);
}
