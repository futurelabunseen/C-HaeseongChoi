// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "SSCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSCharacterNonPlayer : public ASSCharacterBase
{
	GENERATED_BODY()
public:
	ASSCharacterNonPlayer(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void SetDead() override;
};
