// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterNonPlayer.h"
#include "SS_LandBug.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASS_LandBug : public ASSCharacterNonPlayer
{
	GENERATED_BODY()
public:
	ASS_LandBug(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void AttackHitCheck() override;
};
