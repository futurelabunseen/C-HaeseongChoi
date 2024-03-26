// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interface/StratagemInterface.h"
#include "SSStratagem.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStratagem : public UObject, public IStratagemInterface
{
	GENERATED_BODY()
public:
	USSStratagem();

	virtual void ActivateStratagem() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Strata, Meta = (AllowPrivateAccess = "true"))
	TArray<EStrataCommand> ArrCommand;
};
