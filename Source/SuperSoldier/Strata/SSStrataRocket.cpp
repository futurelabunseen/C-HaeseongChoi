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
	ExplosionSystemScale = FVector(1.0f);
	SmokeSystemScale = FVector(1.0f);

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

	// 속도 * 시간으로 이동해야할 거리를 더해 새로운 위치로 설정
    FVector ActorForwardVector = GetActorForwardVector();
    FVector NewLocation = GetActorLocation() + ActorForwardVector * Speed * DeltaTime;
    SetActorLocation(NewLocation, true);
}

void ASSStrataRocket::Strike(const FVector& TargetLocation)
{
	// TargetLocation을 바라보게 액터를 설정하고 Tick을 활성화
    FVector ActorLocation = GetActorLocation();
    FRotator NewRotator = FRotationMatrix::MakeFromX(TargetLocation - ActorLocation).Rotator();
    SetActorRotation(NewRotator);
    SetActorTickEnabled(true);
}

void ASSStrataRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority() && Hit.bBlockingHit)
    {
		// Multicast RPC로 클라이언트에 효과 출력
        NetMulticastRpcPlayEffect(Hit.Location, (Hit.ImpactNormal).ToOrientationRotator());

		// DamageBox와 겹치는 액터에 대미지를 가한다.
        TArray<AActor*> OverlappingActors;
        DamageBox->GetOverlappingActors(OverlappingActors, ASSCharacterBase::StaticClass());
        for (AActor* OverlappingActor : OverlappingActors)
        {
            ASSCharacterBase* OverlappingCharater = Cast<ASSCharacterBase>(OverlappingActor);

            FDamageEvent DamageEvent;
            const float AttackDamage = 2000.0f;
            OverlappingCharater->TakeDamage(AttackDamage, DamageEvent, StrataCauser, this);
        }

		// RPC가 전달되도록 바로 삭제하지 않고, 게임에서 안보이게 한 뒤, 2초 후 삭제 
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
			ExplosionSystemScale);
    }
    
    if (SmokeSystem)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            SmokeSystem,
            Location,
            Rotation,
			SmokeSystemScale);
    }

	if (ExplodeSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(),
			ExplodeSound,
			Location,
			FRotator::ZeroRotator,
			1.0f);
	}
}


