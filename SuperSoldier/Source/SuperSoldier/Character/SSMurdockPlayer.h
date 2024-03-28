// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterPlayer.h"
#include "SSMurdockPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSMurdockPlayer : public ASSCharacterPlayer
{
	GENERATED_BODY()
public:
	ASSMurdockPlayer();
protected:
	virtual void BeginPlay() override;
};
