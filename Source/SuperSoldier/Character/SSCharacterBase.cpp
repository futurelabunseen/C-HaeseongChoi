// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/CharacterStat/SSCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_SSCAPSULE);

	// Mesh
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// Stat
	Stat = CreateDefaultSubobject<USSCharacterStatComponent>(TEXT("Stat"));

	// DissolveCurve
	static ConstructorHelpers::FObjectFinder<UCurveFloat> DissolveCurveFloatRef(
		TEXT("/Game/SuperSoldier/Curves/DissolveCurve.DissolveCurve"));
	if (DissolveCurveFloatRef.Object)
	{
		DissolveCurveFloat = DissolveCurveFloatRef.Object;
	}

	bDead = false;
	CharacterCollisionType = ECharacterCollisionType::Normal;
}

void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (HasAuthority())
	{
		Stat->OnHpZero.AddUObject(this, &ASSCharacterBase::SetDead);
	}

	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();

	// Timeline Binding
	if (DissolveCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ASSCharacterBase::UpdateDissolveProgress);
		DissolveTimeline.AddInterpFloat(DissolveCurveFloat, TimelineProgress);
	}

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
	}
}

void ASSCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DissolveTimeline.TickTimeline(DeltaSeconds);
}

void ASSCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASSCharacterBase, bDead);
	DOREPLIFETIME(ASSCharacterBase, CharacterCollisionType);
}

void ASSCharacterBase::AttackHitCheck(FName AttackId)
{
}

void ASSCharacterBase::PlayMoanSound()
{
}

void ASSCharacterBase::ShowAttackEffect()
{
}

float ASSCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	LastDamageInstigator = EventInstigator;
	NetMulticastRpcShowAnimationMontage(HitReactMontage, 1.0f);
	Stat->ApplyDamage(DamageAmount);
	
	return DamageAmount;
}

void ASSCharacterBase::ReleaseThrowable()
{
}

void ASSCharacterBase::SetDead()
{
	// Server Side Func
	bDead = true;
	CharacterCollisionType = ECharacterCollisionType::NoCollision;

	OnRep_ServerCharacterbDead();
	OnRep_ServerCharacterCollisionType();
}

void ASSCharacterBase::Dissolve()
{
	SetActorTickEnabled(true);
	DissolveTimeline.PlayFromStart();
}

void ASSCharacterBase::UpdateDissolveProgress(const float Value)
{
	for (const auto& DynamicMaterialIndex : DynamicMaterialIndices)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(DynamicMaterialIndex));
		DynamicMaterial->SetScalarParameterValue("DissolveParams", Value);
	}
	if (Value >= 1.f)
	{
		SetActorTickEnabled(false);
	}
}

void ASSCharacterBase::PlayDeadSound()
{
}

void ASSCharacterBase::OnRep_ServerCharacterbDead()
{
	if (bDead)
	{
		bUseControllerRotationYaw = false;

		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(false);
		}
	}
	else
	{
		bUseControllerRotationYaw = true;
	}
}

void ASSCharacterBase::SetCharacterCollisionType(ECharacterCollisionType NewCharacterCollisionType)
{
	CharacterCollisionType = NewCharacterCollisionType;
	OnRep_ServerCharacterCollisionType();
}

void ASSCharacterBase::Respawn(const FVector& TargetLocation)
{
}

void ASSCharacterBase::OnRep_ServerCharacterCollisionType()
{
	if (CharacterCollisionType == ECharacterCollisionType::Normal)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else if (CharacterCollisionType == ECharacterCollisionType::NoCollision)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASSCharacterBase::NetMulticastRpcShowAnimationMontage_Implementation(UAnimMontage* MontageToPlay, const float AnimationSpeedRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(MontageToPlay, AnimationSpeedRate);
}

void ASSCharacterBase::NetMulticastRpcShowAnimationMontageWithSection_Implementation(UAnimMontage* MontageToPlay, FName SectionName, const float AnimationSpeedRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(MontageToPlay, AnimationSpeedRate);

	if (SectionName != TEXT(""))
	{
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
	}
}