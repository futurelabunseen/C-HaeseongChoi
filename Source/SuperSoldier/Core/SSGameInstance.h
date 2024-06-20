// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	USSGameInstance();
	virtual void Init() override;

	class USSStratagemManager* const GetStratagemManager() { return StratagemManager.Get(); }
	class USSStatisticsManager* const GetStatisticsManager() { return StatisticsManager.Get(); }
	USoundConcurrency* const GetDeadSoundConcurrency() const { return DeadSoundConcurrency; }
private:
	UPROPERTY()
	TObjectPtr<class USSStratagemManager> StratagemManager;

	UPROPERTY()
	TObjectPtr<class USSStatisticsManager> StatisticsManager;

	UPROPERTY()
	TObjectPtr<class USoundConcurrency> DeadSoundConcurrency;
};
