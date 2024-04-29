// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterPlayer.h"
#include "SS_MurdockPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASS_MurdockPlayer : public ASSCharacterPlayer
{
	GENERATED_BODY()
public:
	ASS_MurdockPlayer(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void BeginPlay() override;
};
