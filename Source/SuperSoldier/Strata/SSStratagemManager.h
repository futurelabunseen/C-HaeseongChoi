// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SSStratagemManager.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStratagemManager : public UObject
{
	GENERATED_BODY()
public:
	USSStratagemManager();

	void InitializeStratagem();
	class USSStratagem* GetStratagem(const FName& StratagemName);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Strata, Meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<class USSStratagem>> StratagemMap;
};
