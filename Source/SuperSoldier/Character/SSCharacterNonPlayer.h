// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "Interface/SSCharacterAIInterface.h"
#include "SSCharacterNonPlayer.generated.h"
/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSCharacterNonPlayer : public ASSCharacterBase, public ISSCharacterAIInterface
{
	GENERATED_BODY()
public:
	ASSCharacterNonPlayer(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void OnRep_ServerCharacterbDead() override;
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	FAICharacterActionFinished OnActionFinished;

	float DefaultWalkSpeed;
	const float SpeedReductionFactor = 0.1f;

// Attack Section
public:
	void Attack();
	virtual void AttackHitCheck() override;

	void NotifyActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	virtual void SetAIActionDelegate(const FAICharacterActionFinished& InOnActionFinished) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackMontage;

// TurnInPlace Section
public:
	virtual void TurnInPlace(bool bTurnRight) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> TurnInPlaceMontage;

	bool bTurnInPlace;

// AI Section
public:
	virtual float GetAttackRange() override;
protected:
	float AttackRange;

// SFX Section
public:
	virtual void PlaySoundEffect() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USoundBase> AttackSound;
};
