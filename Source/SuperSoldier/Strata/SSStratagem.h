// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SSStratagem.generated.h"

UENUM()
enum class EStrataCommand : uint32
{
	NONE = 0 UMETA(DisplayName = "Command None"),
	UP UMETA(DisplayName = "Command Up"),
	DOWN UMETA(DisplayName = "Command Down"),
	LEFT UMETA(DisplayName = "Command Left"),
	RIGHT UMETA(DisplayName = "Command Right"),
};

UENUM()
enum class EStrataType : int32
{
	NONE,
	CORE,
	SUPPORT,
	OFFENSE,
};

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSStratagem : public UObject
{
	GENERATED_BODY()
public:
	USSStratagem();

	virtual const float& GetDelayTime() const { return DelayTime; }
	virtual const TArray<EStrataCommand>& GetCommandSequence() const;
	virtual void ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector TargetLocation, const FVector ThrowedDirection);

	FORCEINLINE const EStrataType& GetStarataType() const { return StrataType; }
	
protected:
	float DelayTime;
	float CoolTime;

	EStrataType StrataType;
	TArray<EStrataCommand> CommandArray;
};
