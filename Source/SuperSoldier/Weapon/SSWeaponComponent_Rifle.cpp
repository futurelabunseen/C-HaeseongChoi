// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SSWeaponComponent_Rifle.h"
#include "GameFramework/Character.h"
#include "Physics/SSColision.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

USSWeaponComponent_Rifle::USSWeaponComponent_Rifle()
{
	TargetSocketName = TEXT("Socket_MainWeapon");
	MuzzleSocketName = TEXT("Socket_Muzzle");
	ShellEjectSocketName = TEXT("Socket_ShellEject");

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

	static ConstructorHelpers::FObjectFinder<UParticleSystem> HitParticleEffectRef(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Default.P_Default"));
	if (HitParticleEffectRef.Object)
	{
		HitParticleEffect = HitParticleEffectRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MuzzleFlashFXRef(TEXT("/Game/LyraAssets/Effects/Particles/Weapons/NS_WeaponFire_MuzzleFlash_Rifle.NS_WeaponFire_MuzzleFlash_Rifle"));
	if (MuzzleFlashFXRef.Object)
	{
		MuzzleFlashFX = MuzzleFlashFXRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ShellEjectFXRef(TEXT("/Game/LyraAssets/Effects/Particles/Weapons/NS_WeaponFire_ShellEject.NS_WeaponFire_ShellEject"));
	if (ShellEjectFXRef.Object)
	{
		ShellEjectFX = ShellEjectFXRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShellEjectMeshRef(TEXT("/Game/LyraAssets/Effects/Meshes/BulletShells/SM_rifleshell.SM_rifleshell"));
	if (ShellEjectMeshRef.Object)
	{
		ShellEjectMesh = ShellEjectMeshRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TracerFXRef(TEXT("/Game/LyraAssets/Effects/Particles/Weapons/NS_WeaponFire_Tracer.NS_WeaponFire_Tracer"));
	if (TracerFXRef.Object)
	{
		TracerFX = TracerFXRef.Object;
	}
}

const FHitResult USSWeaponComponent_Rifle::AttackHitCheck()
{
	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwner()))
	{
		AController* PlayerController = PlayerCharacter->GetController();

		FVector CameraLocation;
		FRotator CameraRotation;

		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = TraceStart + CameraRotation.Vector() * 5000.0f;

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("Attack")), false, PlayerCharacter);

		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CCHANNEL_SSACTION, TraceParams);

#if ENABLE_DRAW_DEBUG
		FColor DrawColor = HitResult.bBlockingHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DrawColor, false, 5.0f);
#endif
		return HitResult;
	}

	return FHitResult();
}

void USSWeaponComponent_Rifle::ShowAttackEffect(const FHitResult& HitResult)
{
	if (!IsValid(WeaponMesh))
	{
		return;
	}

	// Show Muzzle Flash
	if (WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		FTransform MuzzleSocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
		FRotator MuzzleFlashRotator = MuzzleSocketTransform.GetRotation().Rotator();
		MuzzleFlashRotator.Yaw += 90.0f;

		UNiagaraComponent* MuzzleFlash = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			MuzzleFlashFX,
			MuzzleSocketTransform.GetLocation(),
			MuzzleFlashRotator);

		if (IsValid(MuzzleFlash))
		{
			FVector MuzzleFlashDirection = HitResult.ImpactPoint - MuzzleSocketTransform.GetLocation();
			MuzzleFlash->SetNiagaraVariableVec3(TEXT("User.Direction"), MuzzleFlashDirection);
			MuzzleFlash->SetNiagaraVariableBool(TEXT("User.Trigger"), true);
		}
	}

	// Show ShellEjct
	if (WeaponMesh->DoesSocketExist(ShellEjectSocketName))
	{
		FTransform ShellEjectSocketTransform = WeaponMesh->GetSocketTransform(ShellEjectSocketName);
		FRotator ShellEjectRotator = ShellEjectSocketTransform.GetRotation().Rotator();
		ShellEjectRotator.Yaw += 90.0f;

		UNiagaraComponent* ShellEject = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ShellEjectFX,
			ShellEjectSocketTransform.GetLocation(),
			ShellEjectRotator);

		if (IsValid(ShellEject))
		{
			ShellEject->SetVariableStaticMesh(TEXT("User.ShellEjectStaticMesh"), ShellEjectMesh);
			ShellEject->SetNiagaraVariableBool(TEXT("User.Trigger"), true);
		}
	}

	// Show Tracer
	if (WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		FTransform MuzzleSocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
		FRotator MuzzleRotator = MuzzleSocketTransform.GetRotation().Rotator();
		MuzzleRotator.Yaw += 90.0f;

		UNiagaraComponent* Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			TracerFX,
			MuzzleSocketTransform.GetLocation(),
			MuzzleRotator);

		if (IsValid(Tracer))
		{
			TArray<FVector> ImpactLocations;
			ImpactLocations.Add(HitResult.ImpactPoint);

			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
				Tracer,
				TEXT("User.ImpactPositions"),
				ImpactLocations);

			Tracer->SetNiagaraVariableBool(TEXT("User.Trigger"), true);
		}
	}

	// Show Impact Effect
	if (HitResult.bBlockingHit)
	{
		if (HitParticleEffect)
		{
			FVector Location = HitResult.Location;
			FRotator Rotation = HitResult.ImpactNormal.Rotation();
			Rotation.Pitch -= 90.0f;
			Rotation.Yaw -= 90.0f;
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticleEffect, Location, Rotation);
		}
	}
}
