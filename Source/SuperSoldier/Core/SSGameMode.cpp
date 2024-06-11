// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameMode.h"
#include "Core/SSPlayerController.h"
#include "Core/SSGameState.h"
#include "GameFramework/HUD.h"
#include "EngineUtils.h"
#include "Character/SS_RespawnTankPlayer.h"
#include "Strata/SStrataReinforcements.h"
#include "SuperSoldier.h"


ASSGameMode::ASSGameMode()
{
	// Set Character
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(
		TEXT("/Script/SuperSoldier.SS_RespawnTankPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	// Set PlayerController
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(
		TEXT("/Script/SuperSoldier.SSPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	// Set GameState
	GameStateClass = ASSGameState::StaticClass();
}

void ASSGameMode::StartPlay()
{
	// 게임의 시작을 지시한다.
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::StartPlay();
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ASSGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// 클라이언트의 접속 요청을 처리한다.
	if (CurPlayerNum < MaxPlayerNum)
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
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));

	return NewPlayerController;
}

void ASSGameMode::PostLogin(APlayerController* NewPlayer)
{
	// 플레이어 입장을 위해 플레이어에 필요한 기본 설정을 모두 마무리 한다.
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::PostLogin(NewPlayer);
	SS_LOG(LogSSNetwork, Log, TEXT("%s"), TEXT("End"));
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
			1.0f);
	}
}

void ASSGameMode::RespawnPlayers(FVector TargetLocation)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		ASSCharacterPlayer* PlayerCharacter = CastChecked<ASSCharacterPlayer>(PlayerController->GetCharacter());
		ASSPlayerController* SSPlayerController = CastChecked<ASSPlayerController>(PlayerController);

		if (PlayerCharacter->bDead)
		{
			FVector RespawnLocation = TargetLocation;
			RespawnLocation.Z += 8000.0f;

			ASS_RespawnTankPlayer* RespawnTank = GetWorld()->SpawnActor<ASS_RespawnTankPlayer>(ASS_RespawnTankPlayer::StaticClass());
			RespawnTank->SetActorLocation(RespawnLocation);
			PlayerController->Possess(RespawnTank);

			PlayerCharacter->SetActorHiddenInGame(true);
			PlayerCharacter->SetLifeSpan(2.0f);
		}
	}
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
