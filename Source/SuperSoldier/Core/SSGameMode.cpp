// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameMode.h"
#include "Core/SSPlayerController.h"
#include "Core/SSGameState.h"
#include "Core/SSPlayerState.h"
#include "GameFramework/HUD.h"
#include "EngineUtils.h"
#include "Character/SS_RespawnTankPlayer.h"
#include "Character/SSCharacterNonPlayer.h"
#include "Strata/SStrataReinforcements.h"
#include "SuperSoldier.h"
#include "Kismet/GameplayStatics.h"


ASSGameMode::ASSGameMode()
{
	// Set Character
	DefaultPawnClass = ASS_RespawnTankPlayer::StaticClass();

	// Set PlayerController
	PlayerControllerClass = ASSPlayerController::StaticClass();

	// Set GameState
	GameStateClass = ASSGameState::StaticClass();

	// Set PlayerState
	PlayerStateClass = ASSPlayerState::StaticClass();

	ClearTotalKilledNonPlayerCharacterNum = 500;
}

void ASSGameMode::StartPlay()
{
	// 게임의 시작을 지시한다.
	Super::StartPlay();
}

void ASSGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// 클라이언트의 접속 요청을 처리한다.
	if (CurPlayerNum < MaxPlayerNum && !bWaitingForResetServer)
	{
		++CurPlayerNum;

		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}
	else
	{
		ErrorMessage = TEXT("[PreLogin Failed] Session already FullPlayer");
		FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
	}
}

APlayerController* ASSGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// 접속을 허용한 클라이언트에 대응하는 플레이어 컨트롤러를 만든다.
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	return NewPlayerController;
}

void ASSGameMode::PostLogin(APlayerController* NewPlayer)
{
	// 플레이어 입장을 위해 플레이어에 필요한 기본 설정을 모두 마무리 한다.
	Super::PostLogin(NewPlayer);
}

void ASSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--CurPlayerNum;

	if (bWaitingForResetServer && CurPlayerNum == 0)
	{
		ResetServer();
	}
}

void ASSGameMode::RespawnAllPlayer(FVector TargetLocation)
{
	if (IsAllPlayerDead())
	{
		FTimerHandle RespawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			RespawnTimerHandle, 
			FTimerDelegate::CreateLambda([&]() {
				RespawnPlayers(TargetLocation);
				}),
			1.0f, 
			false, 
			5.0f);
	}
}

int32 ASSGameMode::RespawnPlayers(FVector TargetLocation)
{
	int32 RespawnedPlayerNum = 0;
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		ASSCharacterPlayer* PlayerCharacter = CastChecked<ASSCharacterPlayer>(PlayerController->GetCharacter());
		ASSPlayerController* SSPlayerController = CastChecked<ASSPlayerController>(PlayerController);

		if (PlayerCharacter->bDead)
		{
			FVector RespawnLocation = TargetLocation;
			RespawnLocation.X += FMath::RandRange(-200.0f, 200.0f);
			RespawnLocation.Y += FMath::RandRange(-200.0f, 200.0f);
			RespawnLocation.Z += 8000.0f;

			ASS_RespawnTankPlayer* RespawnTank = GetWorld()->SpawnActor<ASS_RespawnTankPlayer>(ASS_RespawnTankPlayer::StaticClass());
			RespawnTank->SetActorLocation(RespawnLocation);
			RespawnTank->SetRespawnStartCameraLocation(PlayerCharacter->GetFollowCameraWorldLocation());

			PlayerController->Possess(RespawnTank);
			PlayerCharacter->SetActorHiddenInGame(true);
			PlayerCharacter->SetLifeSpan(2.0f);

			RespawnedPlayerNum += 1;
		}
	}

	return RespawnedPlayerNum;
}

bool ASSGameMode::IsAllPlayerDead()
{
	bool bAllPlayerDead = true;
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController)
		{
			ASSCharacterBase* CharacterPlayer = CastChecked<ASSCharacterBase>(PlayerController->GetCharacter());

			if (!CharacterPlayer->bDead)
			{
				bAllPlayerDead = false;
				break;
			}
		}
	}
	return bAllPlayerDead;
}

void ASSGameMode::OnNonPlayerCharacterDead()
{
	ASSGameState* SSGameState = GetGameState<ASSGameState>();
	int32 NewTotalKilledMonsterCount = SSGameState->GetTotalKilledMonsterCount() + 1;
	NewTotalKilledMonsterCount = FMath::Clamp(NewTotalKilledMonsterCount, 0, ClearTotalKilledNonPlayerCharacterNum);
	SSGameState->SetTotalKilledMonsterCount(NewTotalKilledMonsterCount);

	if (NewTotalKilledMonsterCount == ClearTotalKilledNonPlayerCharacterNum)
	{
		for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
		{
			if (PlayerController)
			{
				ASSPlayerController* SSPlayerController = CastChecked<ASSPlayerController>(PlayerController);
				ASSPlayerState* SSPlayerState = SSPlayerController->GetPlayerState<ASSPlayerState>();

				if (IsValid(SSPlayerController) && IsValid(SSPlayerState))
				{
					SSPlayerController->ClientRpcSetAndShowFinalGameStatistics(SSPlayerState->GetPlayStatistics());
				}
			}
		}

		bWaitingForResetServer = true;
		StopServer();
	}
}

void ASSGameMode::SetNonPlayerCharacterSpawn(bool bNewSpawn)
{
	TArray<AActor*> Actors;

	UClass* MonsterSpawnerClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/SuperSoldier/Characters/Monsters/BP_MonsterSpawner.BP_MonsterSpawner_C"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), MonsterSpawnerClass, Actors);
	bool bEnableSpawn = bNewSpawn;

	if (!Actors.IsEmpty())
	{
		for (AActor* Actor : Actors)
		{
			UFunction* SetEnableSpawnFunc = MonsterSpawnerClass->FindFunctionByName(TEXT("SetEnableSpawn"));
			Actor->ProcessEvent(SetEnableSpawnFunc, &bEnableSpawn);
		}
	}
}

void ASSGameMode::SetNonPlayerCharacterStopAI()
{
	TArray<AActor*> Actors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSCharacterNonPlayer::StaticClass(), Actors);

	if (!Actors.IsEmpty())
	{
		for (AActor* Actor : Actors)
		{
			if (ASSCharacterNonPlayer* NonPlayerCharacter = Cast<ASSCharacterNonPlayer>(Actor))
			{
				NonPlayerCharacter->StopAI();
			}
		}
	}
}

void ASSGameMode::StopServer()
{
	SetNonPlayerCharacterStopAI();
	SetNonPlayerCharacterSpawn(false);	
}

void ASSGameMode::ResetServer()
{
	ClearTotalKilledNonPlayerCharacterNum = 0;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSCharacterNonPlayer::StaticClass(), Actors);

	if (!Actors.IsEmpty())
	{
		for (AActor* Actor : Actors)
		{
			if (ASSCharacterNonPlayer* NonPlayerCharacter = Cast<ASSCharacterNonPlayer>(Actor))
			{
				NonPlayerCharacter->Destroy(true);
			}
		}
	}

	SetNonPlayerCharacterSpawn(true);
	bWaitingForResetServer = false;
}
