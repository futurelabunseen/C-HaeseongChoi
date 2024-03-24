// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/SSAnimationAttackInterface.h"
#include "SSCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Normal,
	Aiming
};

UCLASS()
class SUPERSOLDIER_API ASSCharacterBase : public ACharacter, public ISSAnimationAttackInterface
{
	GENERATED_BODY()

public:
	ASSCharacterBase();

// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;
};
