// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPERSOLDIER_API USSCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	USSCharacterStatComponent();
protected:
	virtual void BeginPlay() override;
public:		
	FOnHPZeroDelegate OnHpZero;
	FOnHPChangedDelegate OnHpChanged;

	FORCEINLINE float GetMaxHP() { return MaxHP; }
	FORCEINLINE float GetCurrentHP() { return CurrentHP; }
protected:
	void SetHP(float NewHP);

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHP;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHP;
public:
	float ApplyDamage(float InDamage);
};
