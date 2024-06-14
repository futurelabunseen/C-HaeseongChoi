// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerState.h"
#include "Net/UnrealNetwork.h"

ASSPlayerState::ASSPlayerState()
{
	KilledMonsterCount = 0;
	KilledTeammateCount = 0;
	DeathCount = 0;
	RevivedTeammateCount = 0;
	UsedStratagemCount = 0;
}

void ASSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASSPlayerState, KilledMonsterCount);
	DOREPLIFETIME(ASSPlayerState, KilledTeammateCount);
	DOREPLIFETIME(ASSPlayerState, DeathCount);
	DOREPLIFETIME(ASSPlayerState, RevivedTeammateCount);
	DOREPLIFETIME(ASSPlayerState, UsedStratagemCount);
}
