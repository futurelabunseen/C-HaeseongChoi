// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	ASSCharacterBase();

// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

// Dead Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	virtual void SetDead();
	void PlayDeadAnimation();
	void Dissolve();
	void UpdateDissolveProgress();

	TArray<uint32> DynamicMaterialIndices;
	FTimerHandle DissolveTimerHandle;
	float DissolveDelayTime = 5.0f;
	float DissolveStartTime;
	float DissolveDuration = 6.0f;
};
