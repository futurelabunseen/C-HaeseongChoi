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
	FORCEINLINE FName GetTargetSocketName();
	FORCEINLINE UStaticMeshComponent* GetMesh();
protected:
	UPROPERTY()
	FName TargetSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

// Attack Section
public:
	virtual const FHitResult AttackHitCheck();
	virtual void ShowAttackEffect();
protected:
	UPROPERTY()
	FName MuzzleSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UParticleSystem> MuzzleFlashParticleEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UParticleSystem> HitParticleEffect;
};
