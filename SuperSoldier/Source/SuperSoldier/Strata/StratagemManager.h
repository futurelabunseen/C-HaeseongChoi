// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StratagemManager.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API UStratagemManager : public UObject
{
	GENERATED_BODY()
public:
	UStratagemManager();

	void InitializeStarategem();
private:
	TMap<FString, class IStratagem*> StratagemMap;
};
