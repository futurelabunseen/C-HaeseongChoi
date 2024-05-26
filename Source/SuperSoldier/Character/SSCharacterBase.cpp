// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SSCharacterControlData.h"
#include "Physics/SSColision.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/CharacterStat/SSCharacterStatComponent.h"
#include "UI/SSUserPlayWidget.h"

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
}

void ASSCharacterBase::SetupCharacterWidget(USSUserPlayWidget* InUserWidget)
{
	if (InUserWidget)
	{
		Stat->OnHpChanged.AddUObject(InUserWidget, &USSUserPlayWidget::UpdateHPBar);
		InUserWidget->SetMaxHP(Stat->GetMaxHP());
		InUserWidget->UpdateHPBar(Stat->GetCurrentHP());
	}
}

void ASSCharacterBase::AttackHitCheck()
{
}

void ASSCharacterBase::PlaySoundEffect()
{
}

void ASSCharacterBase::ShowAttackEffect()
{
}

float ASSCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);
	
	return DamageAmount;
}

void ASSCharacterBase::ReleaseThrowable()
{
}

void ASSCharacterBase::SetDead()
{
	bDead = true;
	OnRep_ServerCharacterbDead();

	FTimerHandle RespawnTimerHandle;
	GetGameInstance()->GetTimerManager().SetTimer(RespawnTimerHandle, FTimerDelegate::CreateLambda([&]() {
		UE_LOG(LogTemp, Log, TEXT("Revive"));
		bDead = false;
		Stat->Initialize();
		OnRep_ServerCharacterbDead();
	}), 10.0f, false);
}

void ASSCharacterBase::Dissolve()
{
	SetActorTickEnabled(true);
	DissolveTimeline.PlayFromStart();
}

void ASSCharacterBase::OnDead()
{
	if (bDead)
	{
		bUseControllerRotationYaw = false;

		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(false);
		}

		GetCharacterMovement()->SetMovementMode(MOVE_None);
		SetActorEnableCollision(false);
	}
	else
	{
		bUseControllerRotationYaw = true;

		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		SetActorEnableCollision(true);
	}
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
		Destroy();
	}
}

void ASSCharacterBase::OnRep_ServerCharacterbDead()
{
	OnDead();
}


