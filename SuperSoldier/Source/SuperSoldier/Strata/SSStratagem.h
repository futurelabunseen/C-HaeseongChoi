// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interface/SSStratagemInterface.h"
#include "SSStratagem.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStratagem : public UObject, public ISSStratagemInterface
{
	GENERATED_BODY()
public:
	USSStratagem();

	virtual const float& GetDelayTime() const override { return DelayTime; }
	virtual const TArray<EStrataCommand>& GetCommandSequence() const;
	virtual void ActivateStratagem(UWorld* const CurWorld, const FVector& TargetLocation) override;
protected:
	float DelayTime;
	float CoolTime;

	TArray<EStrataCommand> CommandArray;
};
