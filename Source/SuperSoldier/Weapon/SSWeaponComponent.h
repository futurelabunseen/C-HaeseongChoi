// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SSWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPERSOLDIER_API USSWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USSWeaponComponent();

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION()
	FName GetTargetSocketName();
	
	UFUNCTION()
	UStaticMeshComponent* GetMesh();
protected:
	UPROPERTY()
	FName TargetSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;


// Attack Section
public:
	void LineTraceAttackHitCheck();
	void OnPostAttackHitCheck(const FHitResult& HitResult);
	virtual void AttackHitCheck();
	FORCEINLINE bool GetCanFire() { return bCanFire; }

protected:
	bool bCanFire;
	float FireDelay;
	float AttackDamage;

// VFX Section
public:
	virtual void ShowAttackEffect(const FHitResult& HitResult);
protected:
	UPROPERTY()
	FName MuzzleSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName ShellEjectSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UParticleSystem> HitParticleEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraSystem> MuzzleFlashFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraSystem> ShellEjectFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMesh> ShellEjectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraSystem> TracerFX;

// SFX Section
public:
	virtual void PlaySoundEffect();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USoundBase> ShootSound;

// FX Multicast Func Section
protected:
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastShowFX(const FHitResult& HitResult);

// Recoil Section
public:
	virtual void AddRecoil();
protected:
	float RecoilPitchMin;
	float RecoilPitchMax;
	float RecoilYawMin;
	float RecoilYawMax;
};
