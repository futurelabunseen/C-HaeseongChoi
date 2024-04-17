// Fill out your copyright notice in the Description page of Project Settings.


#include "SSStratagemManager.h"
#include "Interface/SSStratagemInterface.h"
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

ISSStratagemInterface* USSStratagemManager::GetStratagem(const FName& StratagemName)
{
	// 목록에 있는 특정 스트라타젬을 가져와 리턴
	const TScriptInterface<ISSStratagemInterface>& RetStratagem = *StratagemMap.Find(StratagemName);
	ISSStratagemInterface* RetVal = RetStratagem.GetInterface();

	return RetVal;
}
