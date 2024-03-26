// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StratagemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStratagemInterface : public UInterface
{
	GENERATED_BODY()
};

UENUM()
enum class EStrataCommand : uint32
{
	NONE = 0 UMETA(DisplayName = "Command None"),
	UP UMETA(DisplayName = "Command Up"),
	DOWN UMETA(DisplayName = "Command Down"),
	LEFT UMETA(DisplayName = "Command Left"),
	RIGHT UMETA(DisplayName = "Command Right"),
};

/**
 * 
 */
class SUPERSOLDIER_API IStratagemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ActivateStratagem() = 0;
};
