// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterPlayer.h"
#include "SS_RespawnTankPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASS_RespawnTankPlayer : public ASSCharacterPlayer
{
	GENERATED_BODY()
public:
	ASS_RespawnTankPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void Landed(const FHitResult& Hit) override;
protected:
	virtual void BeginPlay() override;
	virtual void OnRep_Controller() override;

public:
	void SetRespawnMurdockCharacter(ACharacter* NewRespawnMurdockCharacter);
protected:
	FTimerHandle RespawnTimerHandle;

	UPROPERTY()
	TObjectPtr<ACharacter> MurdockCharacter;
public:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void RespawnMurdockCharacter();
};
