// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SSWeaponComponent.h"
#include "SSWeaponComponent_Rifle.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSWeaponComponent_Rifle : public USSWeaponComponent
{
	GENERATED_BODY()
	
public:
	USSWeaponComponent_Rifle();

// Attack Section
public:
	virtual void AttackHitCheck() override;

// VFX Section
public:
	virtual void ShowAttackEffect(const FHitResult& HitResult) override;

// SFX Section
public:
	virtual void PlaySoundEffect() override;
};
