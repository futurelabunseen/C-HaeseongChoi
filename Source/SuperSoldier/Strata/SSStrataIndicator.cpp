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
	StrataIndicatorMesh->SetPhysicsLinearVelocity(Direction * ThrowSpeed);
	ThrowedDirection = Direction;
}

void ASSStrataIndicator::SetSimulateCollision()
{
	StrataIndicatorMesh->SetSimulatePhysics(true);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void ASSStrataIndicator::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(CurStratagem)) return;

	// 지연시간 이후 효과 발동
	FTimerHandle StrataActiveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		StrataActiveTimerHandle, 
		this, 
		&ASSStrataIndicator::ActivateStrataAndDestroy, 
		CurStratagem->GetDelayTime(), 
		false);

	EStrataType CurStrataType = CurStratagem->GetStarataType();
	NetMulticastRpcShowStrataBeam(static_cast<int32>(CurStrataType));
}

void ASSStrataIndicator::SetToShowStrataBeam(int32 CurStrataType)
{
	EStrataType CastedStrataType = static_cast<EStrataType>(CurStrataType);

	FVector BeamEnd = GetActorLocation();
	BeamEnd.Z += 8000.0f;

	FLinearColor StrataBeamColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	// 스트라타젬 타입에 따라 빛 색깔을 설정
	switch (CastedStrataType)
	{
	case EStrataType::CORE:
		StrataBeamColor = { 10.0f, 0.0f, 10.0f, 1.0f };
		break;
	case EStrataType::SUPPORT:
		StrataBeamColor = { 0.0f, 0.0f, 10.0f, 1.0f };
		break;
	case EStrataType::OFFENSE:
		StrataBeamColor = { 10.0f, 0.0f, 0.0f, 1.0f };
		break;
	default:
		break;
	}

	StrataIndicatorBeam->SetVectorParameter(TEXT("User.Beam End"), BeamEnd);
	StrataIndicatorBeam->SetColorParameter(TEXT("User.Color"), StrataBeamColor);
	StrataIndicatorBeam->Activate();
}

void ASSStrataIndicator::SetStratagem(USSStratagem* NewStratagem)
{
	CurStratagem = NewStratagem;
}

void ASSStrataIndicator::ActivateStrataAndDestroy()
{
	CurStratagem->ActivateStratagem(GetWorld(), StrataCauser, GetActorLocation(), ThrowedDirection);
	Destroy();
}

void ASSStrataIndicator::NetMulticastRpcShowStrataBeam_Implementation(int32 CurStrataType)
{
	StrataIndicatorMesh->SetSimulatePhysics(false);
	StrataIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!HasAuthority())
	{
		SetToShowStrataBeam(CurStrataType);
	}
}
