// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataIndicator.h"
#include "SuperSoldier.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

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
		StrataIndicatorMesh->SetMassOverrideInKg(NAME_None, 10.0f, true);

		StrataIndicatorMesh->SetSimulatePhysics(false);
		StrataIndicatorMesh->SetGenerateOverlapEvents(false);
		StrataIndicatorMesh->SetNotifyRigidBodyCollision(true);
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
	StrataIndicatorMesh->OnComponentHit.AddDynamic(this, &ASSStrataIndicator::OnHit);

	FLinearColor StrataIndicatorBeamColor(20.0f, 0.0f, 0.0f, 1.0f);
	StrataIndicatorBeam->SetVariableLinearColor(TEXT("User.Color"), StrataIndicatorBeamColor);

	if(HasAuthority())
	{
		StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASSStrataIndicator::Throw(FVector Direction)
{
	const float ThrowSpeed = 2000.0f;
	StrataIndicatorMesh->SetSimulatePhysics(true);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StrataIndicatorMesh->SetPhysicsLinearVelocity(Direction * ThrowSpeed);
}

void ASSStrataIndicator::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector StrataIndicatorBeamEnd = GetActorLocation();
	StrataIndicatorBeamEnd.Z += 2000.0f;

	StrataIndicatorBeam->SetVectorParameter(TEXT("User.Beam End"), StrataIndicatorBeamEnd);
	StrataIndicatorBeam->Activate();

	StrataIndicatorMesh->SetSimulatePhysics(false);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
