// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataRocket.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SuperSoldier.h"
#include "Engine/DamageEvents.h"
#include "Character/SSCharacterBase.h"
#include "Kismet/GameplayStatics.h"

ASSStrataRocket::ASSStrataRocket()
{
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    RocketStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketStaticMesh"));
    RocketStaticMesh->SetUseCCD(true);
    RocketStaticMesh->SetGenerateOverlapEvents(false);
    RocketStaticMesh->SetNotifyRigidBodyCollision(true);
    RootComponent = RocketStaticMesh;

    DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
    DamageBox->SetupAttachment(RootComponent);

    RocketTrailNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RocketTrailNiagara"));
    RocketTrailNiagara->SetupAttachment(RootComponent);

    Speed = 0.0f;

    bReplicates = true;
    SetReplicateMovement(true);
}

ASSStrataRocket::~ASSStrataRocket()
{
}

void ASSStrataRocket::BeginPlay()
{
	Super::BeginPlay();
    SetActorTickEnabled(false);
	SetLifeSpan(20.0f);

    RocketStaticMesh->OnComponentHit.AddDynamic(this, &ASSStrataRocket::OnHit);
}

void ASSStrataRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FVector ActorForwardVector = GetActorForwardVector();
    FVector NewLocation = GetActorLocation() + ActorForwardVector * Speed * DeltaTime;

    SetActorLocation(NewLocation, true);
}

void ASSStrataRocket::Strike(const FVector& TargetLocation)
{
    FVector ActorLocation = GetActorLocation();
    FRotator NewRotator = FRotationMatrix::MakeFromX(TargetLocation - ActorLocation).Rotator();

    SetActorRotation(NewRotator);
    SetActorTickEnabled(true);
}

void ASSStrataRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority() && Hit.bBlockingHit)
    {
        NetMulticastRpcPlayEffect(Hit.Location, (Hit.ImpactNormal).ToOrientationRotator());

        TArray<AActor*> OverlappingActors;
        DamageBox->GetOverlappingActors(OverlappingActors, ASSCharacterBase::StaticClass());

        for (AActor* OverlappingActor : OverlappingActors)
        {
            ASSCharacterBase* OverlappingCharater = Cast<ASSCharacterBase>(OverlappingActor);

            FDamageEvent DamageEvent;
            const float AttackDamage = 2000.0f;
            OverlappingCharater->TakeDamage(AttackDamage, DamageEvent, nullptr, StrataCauser);
        }

        SetActorHiddenInGame(true);
        SetActorTickEnabled(false);
        RocketStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        SetLifeSpan(2.0f);
    }
}

void ASSStrataRocket::NetMulticastRpcPlayEffect_Implementation(FVector_NetQuantize Location, FRotator Rotation)
{
	if (HasAuthority()) return;

    if (ExplosionSystem)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ExplosionSystem,
            Location,
            Rotation,
            FVector(10.0f, 4.0f, 4.0f));
    }
    
    if (SmokeSystem)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            SmokeSystem,
            Location,
            Rotation,
            FVector(10.0f, 4.0f, 4.0f));
    }

	if (ExplodeSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			ExplodeSound,
			Location,
			FRotator::ZeroRotator,
			1.25f);
	}
}


