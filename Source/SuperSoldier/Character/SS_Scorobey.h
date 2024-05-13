// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterNonPlayer.h"
#include "SS_Scorobey.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASS_Scorobey : public ASSCharacterNonPlayer
{
	GENERATED_BODY()
public:
	ASS_Scorobey(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void AttackHitCheck() override;
};
