// Fill out your copyright notice in the Description page of Project Settings.


#include "SSStratagemManager.h"
#include "Strata/SSStratagem.h"
#include "Strata/SSStratPrecisionStrike.h"

USSStratagemManager::USSStratagemManager()
{
}

void USSStratagemManager::InitializeStratagem()
{
	// 스트라타젬 전체 목록 초기화
	USSStratPrecisionStrike* Stratagem = NewObject<USSStratPrecisionStrike>();
	if (Stratagem)
	{
		StratagemMap.Add(FName(TEXT("PrecisionStrike")), Stratagem);
	}
}

USSStratagem* USSStratagemManager::GetStratagem(const FName& StratagemName)
{
	// 목록에 있는 특정 스트라타젬을 가져와 리턴
	USSStratagem* RetStratagem = *StratagemMap.Find(StratagemName);
	return RetStratagem;
}
