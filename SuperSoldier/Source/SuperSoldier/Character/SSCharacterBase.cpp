// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_SSCAPSULE);

	// Mesh
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASSCharacterBase::AttackHitCheck()
{
}

float ASSCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	SetDead();

	return DamageAmount;
}

void ASSCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void ASSCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.5f);
}

void ASSCharacterBase::Dissolve()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();

	if (SkeletalMeshComponent)
	{
		TArray<UMaterialInterface*> Materials = SkeletalMeshComponent->GetMaterials();

		uint32 MaterialIndex = 0;
		for (UMaterialInterface* const Material : Materials)
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			if (DynamicMaterial)
			{
				SkeletalMeshComponent->SetMaterial(MaterialIndex, DynamicMaterial);
				DynamicMaterialIndices.Add(MaterialIndex);
			}
			++MaterialIndex;
		}

		if (!DynamicMaterialIndices.IsEmpty())
		{
			const float TimerRate = 0.022f;		// 10초 45프레임 업데이트
			DissolveStartTime = GetWorld()->GetTimeSeconds();
			GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &ASSCharacterBase::UpdateDissolveProgress, TimerRate, true);
		}
	}
}

void ASSCharacterBase::UpdateDissolveProgress()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - DissolveStartTime;
	float Alpha = FMath::Clamp(ElapsedTime / DissolveDuration, 0.0f, 1.0f);

	for (const int32& DynamicMaterialIndex : DynamicMaterialIndices)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(DynamicMaterialIndex));
		DynamicMaterial->SetScalarParameterValue("DissolveParams", Alpha);
	}

	if (Alpha >= 1.0f)
	{
		GetWorldTimerManager().ClearTimer(DissolveTimerHandle);
		Destroy();
	}
}


