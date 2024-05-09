// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Interface/SSAnimationAttackInterface.h"
#include "SSCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Normal,
	Aiming
};

UCLASS()
class SUPERSOLDIER_API ASSCharacterBase : public ACharacter, public ISSAnimationAttackInterface
{
	GENERATED_BODY()

public:
	ASSCharacterBase(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Stat Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USSCharacterStatComponent> Stat;

// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

// Throw Section
public:
	virtual void ReleaseThrowable();

// Dead Section
protected:
	void SetDead();
	void Dissolve();

	UFUNCTION()
	virtual void OnDead();

	UFUNCTION()
	void UpdateDissolveProgress(const float Value);

protected:
	const float DissolveDelayTime = 5.0f;
	TArray<uint32> DynamicMaterialIndices;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline DissolveTimeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	TObjectPtr<UCurveFloat> DissolveCurveFloat;

public:
	UPROPERTY(ReplicatedUsing = OnRep_ServerCharacterbDead)
	uint8 bDead : 1;

protected:
	UFUNCTION()
	void OnRep_ServerCharacterbDead();
};
