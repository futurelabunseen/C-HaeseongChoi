// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameState.h"
#include "SuperSoldier.h"
#include "Net/UnrealNetwork.h"

void ASSGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();
}

void ASSGameState::OnRep_ReplicatedHasBegunPlay()
{
	Super::OnRep_ReplicatedHasBegunPlay();
}

void ASSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASSGameState, TotalKilledMonsterCount);
}

void ASSGameState::OnRep_TotalKilledMonsterCount()
{
	SS_LOG(LogSSNetwork, Log, TEXT("TotalKilledMonsterCount : %d"), TotalKilledMonsterCount);
	OnTotalKilledMonsterCountChangedDelegate.Broadcast(TotalKilledMonsterCount);
}
