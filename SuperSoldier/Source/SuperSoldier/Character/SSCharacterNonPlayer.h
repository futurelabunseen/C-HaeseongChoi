// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "SSCharacterNonPlayer.generated.h"

DECLARE_DELEGATE(FAICharacterAttackFinished);

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSCharacterNonPlayer : public ASSCharacterBase
{
	GENERATED_BODY()
public:
	ASSCharacterNonPlayer(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void SetDead() override;

public:
	void Attack();
	void AttackHitCheck() override;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastRpcShowAttackAnimation();

	void NotifyAttackActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished);
protected:
	FAICharacterAttackFinished OnAttackFinished;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackMontage;
};
