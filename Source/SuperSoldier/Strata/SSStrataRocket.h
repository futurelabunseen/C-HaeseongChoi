// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSStrataRocket.generated.h"

UCLASS()
class SUPERSOLDIER_API ASSStrataRocket : public AActor
{
	GENERATED_BODY()
	
public:	
	ASSStrataRocket();
    ~ASSStrataRocket();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class UStaticMeshComponent> RocketStaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class UBoxComponent> DamageBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class UNiagaraComponent> RocketTrailNiagara;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UNiagaraSystem> ExplosionSystem;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UNiagaraSystem> SmokeSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USoundBase> ExplodeSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Speed;
public:
    UFUNCTION()
    virtual void Strike(const FVector& TargetLocation);

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION(NetMulticast, Unreliable)
    void NetMulticastRpcPlayEffect(FVector_NetQuantize Location, FRotator Rotation);
public:
	FORCEINLINE void SetStrataCauser(AActor* NewStrataCauser) { StrataCauser = NewStrataCauser; }
protected:
	AActor* StrataCauser;
};
