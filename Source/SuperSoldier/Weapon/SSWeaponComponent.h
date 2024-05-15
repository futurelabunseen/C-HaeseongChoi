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
	FORCEINLINE UStaticMeshComponent* GetMesh();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

// Attack Section
protected:
	virtual bool AttackHitCheck();
};
