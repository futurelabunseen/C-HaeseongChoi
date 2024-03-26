// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interface/StratagemInterface.h"
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

	void InitializeStratagem();
	IStratagemInterface* GetStratagem(const FName& StratagemName);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Strata, Meta = (AllowPrivateAccess = "true"))
	TMap<FName, IStratagemInterface*> StratagemMap;
};
