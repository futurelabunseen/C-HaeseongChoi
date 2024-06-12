// Fill out your copyright notice in the Description page of Project Settings.


#include "SSStratagemManager.h"
#include "Strata/SSStratagem.h"
#include "Strata/SStrataReinforcements.h"
#include "Strata/SSStrataPrecisionStrike.h"
#include "Strata/SSStrataAirStrike.h"

USSStratagemManager::USSStratagemManager()
{
}

void USSStratagemManager::InitializeStratagem()
{
	// 스트라타젬 전체 목록 초기화
	USStrataReinforcements* Reinforcements = NewObject<USStrataReinforcements>();
	if (Reinforcements)
	{
		StratagemMap.Add(FName(TEXT("Reinforcements")), Reinforcements);
	}

	USSStrataPrecisionStrike* PrecisionStrike = NewObject<USSStrataPrecisionStrike>();
	if (PrecisionStrike)
	{
		StratagemMap.Add(FName(TEXT("PrecisionStrike")), PrecisionStrike);
	}

	USSStrataAirStrike* AirStrike = NewObject<USSStrataAirStrike>();
	if (AirStrike)
	{
		StratagemMap.Add(FName(TEXT("AirStrike")), AirStrike);
	}
}

USSStratagem* USSStratagemManager::GetStratagem(const FName& StratagemName)
{
	// 목록에 있는 특정 스트라타젬을 가져와 리턴
	USSStratagem* RetStratagem = *StratagemMap.Find(StratagemName);
	return RetStratagem;
}
