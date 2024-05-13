// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSStrataIndicator.generated.h"

UCLASS()
class SUPERSOLDIER_API ASSStrataIndicator : public AActor
{
	GENERATED_BODY()
public:	
	ASSStrataIndicator();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> StrataIndicatorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> StrataIndicatorBeam;
protected:
	virtual void BeginPlay() override;
protected:
	UFUNCTION()
	void Throw(FVector Direction);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetToShowStrataBeam(FVector BeamEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastRpcShowStrataBeam(FVector_NetQuantize BeamEnd);
protected:
	class USSStratagem* CurStratagem;
public:
	void SetStratagem(class USSStratagem* NewStratagem);
	void ActivateStrataAndDestroy();
};
