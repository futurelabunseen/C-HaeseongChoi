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
	UPROPERTY()
	FVector ThrowedDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> StrataIndicatorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> StrataIndicatorBeam;
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void Throw(FVector Direction);

	UFUNCTION()
	void SetSimulateCollision();
protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetToShowStrataBeam(FLinearColor BeamColor, FVector BeamEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastRpcShowStrataBeam(FLinearColor BeamColor, FVector_NetQuantize BeamEnd);
public:
	void SetStratagem(class USSStratagem* NewStratagem);
	void ActivateStrataAndDestroy();
protected:
	class USSStratagem* CurStratagem;
public:
	FORCEINLINE void SetStrataCauser(AController* NewStrataCauser) { StrataCauser = NewStrataCauser; }
protected:
	AController* StrataCauser;
};
