// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterNonPlayer.h"
#include "SS_Kraken.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASS_Kraken : public ASSCharacterNonPlayer
{
	GENERATED_BODY()
public:
	ASS_Kraken(const FObjectInitializer& ObjectInitializer);
protected:
	const float MeshScale = 0.4f;
protected:
	virtual void AttackHitCheck() override;
	virtual void Tick(float DeltaSeconds) override;

// TurnInPlace Section
public:
	virtual void TurnInPlace(bool bTurnRight) override;
};
