// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASSGameMode();
	virtual void StartPlay() override;

// Login Section
public:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	const uint32 MaxPlayerNum = 2;
	uint32 CurPlayerNum;
};
