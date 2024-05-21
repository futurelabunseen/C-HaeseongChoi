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
	virtual const FHitResult AttackHitCheck() override;

// VFX Section
	virtual void ShowAttackEffect(const FHitResult& HitResult) override;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastShowVFX(const FHitResult& HitResult);

// SFX Section
public:
	virtual void PlaySoundEffect() override;
};
