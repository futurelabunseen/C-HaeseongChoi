// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SSCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Normal,
	Aiming
};

UCLASS()
class SUPERSOLDIER_API ASSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASSCharacterBase();

protected:
	virtual void SetCharacterControlData(const class USSCharacterControlData* CharacterControlData);

	TMap<ECharacterControlType, class USSCharacterControlData*> CharacterControlManager;
};
