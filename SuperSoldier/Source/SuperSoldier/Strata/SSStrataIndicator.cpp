// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "SuperSoldier.h"

ASSStrataIndicator::ASSStrataIndicator()
{
	StrataIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StrataIndicatorMesh"));

	RootComponent = StrataIndicatorMesh;

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

	bReplicates = true;
	SetReplicateMovement(true);
}

void ASSStrataIndicator::BeginPlay()
{
	StrataIndicatorMesh->OnComponentHit.AddDynamic(this, &ASSStrataIndicator::OnHit);

	if(HasAuthority())
	{
		StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASSStrataIndicator::Throw(FVector Direction)
{
	StrataIndicatorMesh->SetSimulatePhysics(true);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StrataIndicatorMesh->SetPhysicsLinearVelocity(Direction * 2000.0f);
}

void ASSStrataIndicator::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	StrataIndicatorMesh->SetSimulatePhysics(false);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
