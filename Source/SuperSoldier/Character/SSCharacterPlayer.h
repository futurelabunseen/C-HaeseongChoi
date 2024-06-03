// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "InputActionValue.h"
#include "Strata/SSStratagem.h"
#include "SSCharacterPlayer.generated.h"

/**
 * 
 */

UCLASS()
class SUPERSOLDIER_API ASSCharacterPlayer : public ASSCharacterBase
{
	GENERATED_BODY()
public:
	ASSCharacterPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void Respawn(const FVector& TargetLocation);
protected:
	bool GetAnyMontagePlaying(UAnimMontage* FilterMontage = NULL);

// Camera Section
protected:
	// 지지대 역할을 하는 스프링암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// RPC Section
protected:
	void RpcPlayAnimation(UAnimMontage* MontageToPlay);
	void RpcJumpToSection(UAnimMontage* MontageToPlay, FName SectionName);

	UFUNCTION(Client, Unreliable)
	void ClientRpcPlayAnimation(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay);

	UFUNCTION(Client, Unreliable)
	void ClientRpcJumpToSection(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay, FName SectionName);

	virtual void Tick(float DeltaSeconds) override;
};
