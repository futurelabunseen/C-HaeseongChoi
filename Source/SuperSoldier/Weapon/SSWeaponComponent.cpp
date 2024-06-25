// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SSWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Physics/SSColision.h"
#include "Engine/DamageEvents.h"
#include "SuperSoldier.h"

// Sets default values for this component's properties
USSWeaponComponent::USSWeaponComponent()
{
	bCanFire = true;
	FireDelay = 0.0f;
	AttackDamage = 0.0f;
	
	RecoilPitchMin = 0.0f;
	RecoilPitchMax = 0.0f;
	RecoilYawMin = 0.0f;
	RecoilYawMax = 0.0f;
}

void USSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);

	if (WeaponMesh)
	{
		WeaponMesh->RegisterComponent();
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

FName USSWeaponComponent::GetTargetSocketName()
{
	return TargetSocketName;
}

UStaticMeshComponent* USSWeaponComponent::GetMesh()
{
	return WeaponMesh;
}

void USSWeaponComponent::LineTraceAttackHitCheck()
{
	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwner()))
	{
		AController* PlayerController = PlayerCharacter->GetController();

		if (PlayerController)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

			// 화면 중앙으로 향하는 벡터를 이용해 충돌을 검사
			FVector TraceStart = CameraLocation;
			FVector TraceEnd = TraceStart + CameraRotation.Vector() * 5000.0f;

			FHitResult HitResult;
			FCollisionQueryParams TraceParams(FName(TEXT("Attack")), false, PlayerCharacter);

			GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CCHANNEL_SSACTION, TraceParams);
			OnPostAttackHitCheck(HitResult);

//#if ENABLE_DRAW_DEBUG
//			FColor DrawColor = HitResult.bBlockingHit ? FColor::Green : FColor::Red;
//			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DrawColor, false, 5.0f);
//#endif
		}
	}
}

void USSWeaponComponent::OnPostAttackHitCheck(const FHitResult& HitResult)
{
	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (PlayerCharacter->IsLocallyControlled())
		{
			ShowAttackEffect(HitResult);
			PlaySoundEffect();
			AddRecoil();
		}

		if (PlayerCharacter->HasAuthority())
		{
			NetMulticastShowFX(HitResult);
			
			if (HitResult.bBlockingHit)
			{
				FDamageEvent DamageEvent;
				HitResult.GetActor()->TakeDamage(
					AttackDamage, 
					DamageEvent, 
					PlayerCharacter->GetController(), 
					PlayerCharacter);
			}
		}
	}
}

void USSWeaponComponent::AttackHitCheck()
{
	
}

void USSWeaponComponent::ShowAttackEffect(const FHitResult& HitResult)
{
}

void USSWeaponComponent::PlaySoundEffect()
{
}

void USSWeaponComponent::AddRecoil()
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwner());
	check(PlayerCharacter);

	APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	check(PlayerController);

	PlayerController->AddPitchInput(FMath::FRandRange(-RecoilPitchMin, RecoilPitchMax));
	PlayerController->AddYawInput(FMath::FRandRange(-RecoilYawMin, RecoilYawMax));
}

void USSWeaponComponent::NetMulticastShowFX_Implementation(const FHitResult& HitResult)
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwner());
	if (PlayerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ShowAttackEffect(HitResult);
		PlaySoundEffect();
	}
}
