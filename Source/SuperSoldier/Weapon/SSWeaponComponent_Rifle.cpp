// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SSWeaponComponent_Rifle.h"
#include "GameFramework/Character.h"
#include "Physics/SSColision.h"

USSWeaponComponent_Rifle::USSWeaponComponent_Rifle()
{
	TargetSocketName = TEXT("Socket_MainWeapon");

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
