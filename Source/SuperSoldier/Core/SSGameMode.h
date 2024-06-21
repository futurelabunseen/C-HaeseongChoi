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
	virtual void Logout(AController* Exiting) override;
protected:
	bool bWaitingForResetServer = false;
	const uint32 MaxPlayerNum = 3;
	uint32 CurPlayerNum = 0;

// Respawn Section
public:
	UFUNCTION()
	int32 RespawnPlayers(FVector TargetLocation);

protected:
	bool IsAllPlayerDead();

// Game Clear Section
public:
	void OnNonPlayerCharacterDead();
	void OnPlayerCharacterDead(FVector DeadLocation);
	void SetNonPlayerCharacterSpawn(bool bNewSpawn);
	void SetNonPlayerCharacterStopAI();
	void EndServer(bool bCleared);
	void StopServer();
	void ResetServer();
protected:
	const int32 ClearTotalKilledNonPlayerCharacterNum = 200;
	const int32 PlayerRespawnLimits = 10;
};
