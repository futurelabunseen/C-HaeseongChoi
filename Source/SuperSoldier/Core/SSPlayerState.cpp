// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSPlayerState.h"
#include "Net/UnrealNetwork.h"

ASSPlayerState::ASSPlayerState()
{
	PlayStatistics.KilledMonsterCount = 0;
	PlayStatistics.KilledTeammateCount = 0;
	PlayStatistics.DeathCount = 0;
	PlayStatistics.RevivedTeammateCount = 0;
	PlayStatistics.UsedStratagemCount = 0;
}