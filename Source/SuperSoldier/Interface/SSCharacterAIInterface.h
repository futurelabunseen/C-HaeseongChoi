// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SSCharacterAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USSCharacterAIInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE(FAICharacterActionFinished);

/**
 * 
 */
class SUPERSOLDIER_API ISSCharacterAIInterface
{
	GENERATED_BODY()
// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetAIActionDelegate(const FAICharacterActionFinished& InOnActionFinished) = 0;
	virtual void TurnInPlace(bool bTurnRight) = 0;

	virtual const float& GetAttackRange() const = 0;
	virtual const TArray<FName>& GetAttackMontageSectionNames() const = 0;
};
