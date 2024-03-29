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

	UFUNCTION()
	class USSStratagemManager* const GetStratagemManager() { return StratagemManager.Get(); }
private:
	UPROPERTY()
	TObjectPtr<class USSStratagemManager> StratagemManager;
};
