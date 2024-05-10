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
	// Delay And Activate
	FTimerHandle StrataActiveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StrataActiveTimerHandle, this, &ASSStrataIndicator::ActivateStrataAndDestroy, CurStratagem->GetDelayTime(), false);

	FVector StrataIndicatorBeamEnd = GetActorLocation();
	StrataIndicatorBeamEnd.Z += 8000.0f;
	NetMulticastRpcShowStrataBeam(StrataIndicatorBeamEnd);
}

void ASSStrataIndicator::SetToShowStrataBeam(FVector BeamEnd)
{
	StrataIndicatorBeam->SetVectorParameter(TEXT("User.Beam End"), BeamEnd);
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

void ASSStrataIndicator::NetMulticastRpcShowStrataBeam_Implementation(FVector_NetQuantize BeamEnd)
{
	SetToShowStrataBeam(BeamEnd);

	StrataIndicatorMesh->SetSimulatePhysics(false);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
