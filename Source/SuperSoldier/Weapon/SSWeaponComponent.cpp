// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SSWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SuperSoldier.h"

// Sets default values for this component's properties
USSWeaponComponent::USSWeaponComponent()
{
}

void USSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);

	if (WeaponMesh)
	{
		WeaponMesh->RegisterComponent();
	}
}

FName USSWeaponComponent::GetTargetSocketName()
{
	return FName();
}

UStaticMeshComponent* USSWeaponComponent::GetMesh()
{
	return WeaponMesh;
}

const FHitResult USSWeaponComponent::AttackHitCheck()
{
	return FHitResult();
}
