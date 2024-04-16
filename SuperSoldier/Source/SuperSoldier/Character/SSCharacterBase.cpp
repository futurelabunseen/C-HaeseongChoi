// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

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

	// DissolveCurve
	static ConstructorHelpers::FObjectFinder<UCurveFloat> DissolveCurveFloatRef(
		TEXT("/Game/SuperSoldier/Curves/DissolveCurve.DissolveCurve"));
	if (DissolveCurveFloatRef.Object)
	{
		DissolveCurveFloat = DissolveCurveFloatRef.Object;
	}

	bDead = false;
}

void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

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
}

void ASSCharacterBase::AttackHitCheck()
{
}

float ASSCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	bDead = true;
	OnRep_ServerCharacterbDead();
	return DamageAmount;
}

void ASSCharacterBase::ReleaseThrowable()
{
}

void ASSCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);
}

void ASSCharacterBase::Dissolve()
{
	SetActorTickEnabled(true);
	DissolveTimeline.PlayFromStart();
}

void ASSCharacterBase::UpdateDissolveProgress(const float Value)
{
	const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
	for (const auto& DynamicMaterialIndex : DynamicMaterialIndices)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(DynamicMaterialIndex));
		DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
	}
	if (Lerp >= 1.f)
	{
		Destroy();
	}
}

void ASSCharacterBase::OnRep_ServerCharacterbDead()
{
	if (bDead)
	{
		SetDead();
	}
}


