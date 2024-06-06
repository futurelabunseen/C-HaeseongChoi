// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SSAnimationPlaySoundInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USSAnimationPlaySoundInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SUPERSOLDIER_API ISSAnimationPlaySoundInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PlayMoanSound() = 0;
};
