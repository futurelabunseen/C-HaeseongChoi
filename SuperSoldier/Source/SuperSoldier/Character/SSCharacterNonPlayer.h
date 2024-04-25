// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "SSCharacterNonPlayer.generated.h"

DECLARE_DELEGATE(FAICharacterActionFinished);

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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void SetDead() override;
protected:
	FAICharacterActionFinished OnActionFinished;

// Attack Section
public:
	void Attack();
	void AttackHitCheck() override;

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastRpcShowAttackAnimation();

	void NotifyActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	virtual void SetAIActionDelegate(const FAICharacterActionFinished& InOnActionFinished);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackMontage;

// TurnInPlace Section
public:
	void TurnInPlace(bool bTurnRight);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> TurnInPlaceMontage;

	UPROPERTY(EditAnywhere)
	FTimeline TurnInPlaceTimeline;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> TurnInPlaceCurveFloat;

	FRotator TurnInPlaceBeginRotation;

	UFUNCTION()
	void UpdateTurnInPlaceProgress(const float Value);
};
