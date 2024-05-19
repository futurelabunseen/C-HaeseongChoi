// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SSWeaponComponent_Rifle.h"
#include "GameFramework/Character.h"
#include "Physics/SSColision.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

USSWeaponComponent_Rifle::USSWeaponComponent_Rifle()
{
	TargetSocketName = TEXT("Socket_MainWeapon");
	MuzzleSocketName = TEXT("Socket_Muzzle");
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

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleFlashParticleRef(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Default.P_Default"));
	if (MuzzleFlashParticleRef.Object)
	{
		MuzzleFlashParticleEffect = MuzzleFlashParticleRef.Object;
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

#if ENABLE_DRAW_DEBUG
		FColor DrawColor = HitResult.bBlockingHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DrawColor, false, 5.0f);
#endif
		return HitResult;
	}

	return FHitResult();
}

void USSWeaponComponent_Rifle::ShowAttackEffect()
{
	if (WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), 
			MuzzleFlashParticleEffect, 
			SocketTransform.GetLocation(), 
			SocketTransform.GetRotation().Rotator());
	}
}
