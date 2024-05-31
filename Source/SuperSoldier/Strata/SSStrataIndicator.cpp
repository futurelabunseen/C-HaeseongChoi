// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataIndicator.h"
#include "SuperSoldier.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Strata/SSStratagem.h"

ASSStrataIndicator::ASSStrataIndicator()
{
	StrataIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StrataIndicatorMesh"));
	RootComponent = StrataIndicatorMesh;

	StrataIndicatorBeam = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StrataIndicatorBeam"));
	StrataIndicatorBeam->SetupAttachment(RootComponent);

	// Add Static Mesh Component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StrataIndicatorMeshRef(TEXT("/Game/SuperSoldier/Props/SM_StrataIndicator.SM_StrataIndicator"));
	if (StrataIndicatorMeshRef.Object)
	{
		StrataIndicatorMesh->SetStaticMesh(StrataIndicatorMeshRef.Object);

		StrataIndicatorMesh->SetSimulatePhysics(false);
		StrataIndicatorMesh->SetGenerateOverlapEvents(false);
		StrataIndicatorMesh->SetNotifyRigidBodyCollision(false);
		StrataIndicatorMesh->SetCollisionProfileName(TEXT("SSStrataIndicator"));
	}

	// Add Niagara Component
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> StrataIndicatorBeamRef(TEXT("/Game/SuperSoldier/Props/NS_StrataIndicatorBeam.NS_StrataIndicatorBeam"));
	if (StrataIndicatorBeamRef.Object)
	{
		StrataIndicatorBeam->SetAsset(StrataIndicatorBeamRef.Object);
		StrataIndicatorBeam->SetAutoActivate(false);
	}

	bReplicates = true;
	SetReplicateMovement(true);
}

void ASSStrataIndicator::BeginPlay()
{
	Super::BeginPlay();

	StrataIndicatorMesh->OnComponentHit.AddDynamic(this, &ASSStrataIndicator::OnHit);
	StrataIndicatorMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);

	FLinearColor StrataIndicatorBeamColor(20.0f, 0.0f, 0.0f, 1.0f);
	StrataIndicatorBeam->SetVariableLinearColor(TEXT("User.Color"), StrataIndicatorBeamColor);

	if(HasAuthority())
	{
		StrataIndicatorMesh->SetNotifyRigidBodyCollision(true);
		StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASSStrataIndicator::Throw(FVector Direction)
{
	const float ThrowSpeed = 2000.0f;
	StrataIndicatorMesh->SetSimulatePhysics(true);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StrataIndicatorMesh->SetPhysicsLinearVelocity(Direction * ThrowSpeed);
}

void ASSStrataIndicator::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(CurStratagem)) return;

	// Delay And Activate
	FTimerHandle StrataActiveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StrataActiveTimerHandle, this, &ASSStrataIndicator::ActivateStrataAndDestroy, CurStratagem->GetDelayTime(), false);

	FVector StrataIndicatorBeamEnd = GetActorLocation();
	EStrataType CurStrataType = CurStratagem->GetStarataType();
	FLinearColor StrataBeamColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	StrataIndicatorBeamEnd.Z += 8000.0f;

	switch (CurStrataType)
	{
	case EStrataType::SUPPORT:
		StrataBeamColor = { 0.0f, 0.0f, 10.0f, 1.0f };
		break;
	case EStrataType::OFFENSE:
		StrataBeamColor = { 10.0f, 0.0f, 0.0f, 1.0f };
		break;
	default:
		break;
	}

	NetMulticastRpcShowStrataBeam(StrataBeamColor, StrataIndicatorBeamEnd);
}

void ASSStrataIndicator::SetToShowStrataBeam(FLinearColor BeamColor, FVector BeamEnd)
{
	StrataIndicatorBeam->SetVectorParameter(TEXT("User.Beam End"), BeamEnd);
	StrataIndicatorBeam->SetColorParameter(TEXT("User.Color"), BeamColor);
	StrataIndicatorBeam->Activate();
}

void ASSStrataIndicator::SetStratagem(USSStratagem* NewStratagem)
{
	CurStratagem = NewStratagem;
}

void ASSStrataIndicator::ActivateStrataAndDestroy()
{
	CurStratagem->ActivateStratagem(GetWorld(), GetActorLocation());
	Destroy();
}

void ASSStrataIndicator::NetMulticastRpcShowStrataBeam_Implementation(FLinearColor BeamColor, FVector_NetQuantize BeamEnd)
{
	StrataIndicatorMesh->SetSimulatePhysics(false);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!HasAuthority())
	{
		SetToShowStrataBeam(BeamColor, BeamEnd);
	}
}
