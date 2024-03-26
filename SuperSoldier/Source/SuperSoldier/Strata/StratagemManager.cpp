// Fill out your copyright notice in the Description page of Project Settings.


#include "StratagemManager.h"
#include "Interface/StratagemInterface.h"
#include "SSStratagem.h"

UStratagemManager::UStratagemManager()
{
	InitializeStratagem();
}

void UStratagemManager::InitializeStratagem()
{
	// 스트라타젬 전체 목록 초기화
	USSStratagem* Stratagem = NewObject<USSStratagem>();
	if (Stratagem)
	{
		StratagemMap.Add(FName(TEXT("Stratagem")), Stratagem);
	}
}

IStratagemInterface* UStratagemManager::GetStratagem(const FName& StratagemName)
{
	// 목록에 있는 특정 스트라타젬을 가져와 리턴
	IStratagemInterface* RetVal = *StratagemMap.Find(StratagemName);

	return RetVal;
}
