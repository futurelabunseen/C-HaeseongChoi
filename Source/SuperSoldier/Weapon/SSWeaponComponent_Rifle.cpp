// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SSWeaponComponent_Rifle.h"

USSWeaponComponent_Rifle::USSWeaponComponent_Rifle()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(this);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponMeshRef(TEXT("/Game/Sci_Fi_Weapon_Bundle/SciFiWeaponsPack/StaticMeshes/SM_RifleWhite.SM_RifleWhite"));
	if (WeaponMeshRef.Object)
	{
		WeaponMesh->SetStaticMesh(WeaponMeshRef.Object);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetGenerateOverlapEvents(false);
		WeaponMesh->SetNotifyRigidBodyCollision(false);
	}
}

bool USSWeaponComponent_Rifle::AttackHitCheck()
{
	return false;
}
