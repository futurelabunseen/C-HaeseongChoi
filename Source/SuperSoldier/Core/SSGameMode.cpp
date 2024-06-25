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
}

void ASSGameMode::StartPlay()
{
	// 게임의 시작을 지시한다.
	Super::StartPlay();
	ResetServer();
}

void ASSGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// 클라이언트의 접속 요청을 처리한다.
	// 최대 플레이어 수가 되었거나 서버 리셋을 기다리는 중이라면 접속 거부
	if (CurPlayerNum < MaxPlayerNum && !bWaitingForResetServer)
	{
		++CurPlayerNum;

		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}
	else
	{
		ErrorMessage = TEXT("[PreLogin Failed] Session Rejected");
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

	if (CurPlayerNum == 0)
	{
		bWaitingForResetServer = true;
		ResetServer();
	}
}

int32 ASSGameMode::RespawnPlayers(FVector TargetLocation)
{
	ASSGameState* SSGameState = GetGameState<ASSGameState>();

	int32 RespawnedPlayerNum = 0;
	int32 CurPlayerRespawnRemain = SSGameState->GetRemainPlayerRespawnCount();

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		ASSCharacterPlayer* PlayerCharacter = CastChecked<ASSCharacterPlayer>(PlayerController->GetCharacter());
		ASSPlayerController* SSPlayerController = CastChecked<ASSPlayerController>(PlayerController);

		if (PlayerCharacter->bDead && (CurPlayerRespawnRemain > 0))
		{
			FVector RespawnLocation = TargetLocation;
			RespawnLocation.X += FMath::RandRange(-200.0f, 200.0f);
			RespawnLocation.Y += FMath::RandRange(-200.0f, 200.0f);
			RespawnLocation.Z += 8000.0f;

			// 리스폰 탱크를 스폰, 현재 카메라 위치 정보를 전달해준뒤, Possess하게 한다.
			ASS_RespawnTankPlayer* RespawnTank = GetWorld()->SpawnActor<ASS_RespawnTankPlayer>(ASS_RespawnTankPlayer::StaticClass());
			RespawnTank->SetActorLocation(RespawnLocation);
			RespawnTank->SetRespawnStartCameraLocation(PlayerCharacter->GetFollowCameraWorldLocation());

			PlayerController->Possess(RespawnTank);

			// 원래 캐릭터는 게임에서 안보이게 한 뒤, 2초 후 삭제
			PlayerCharacter->SetActorHiddenInGame(true);
			PlayerCharacter->SetLifeSpan(2.0f);

			RespawnedPlayerNum += 1;
			--CurPlayerRespawnRemain;
		}
	}

	SSGameState->SetRemainPlayerRespawnCount(CurPlayerRespawnRemain);

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
		EndServer(true);
	}
}

void ASSGameMode::OnPlayerCharacterDead(FVector DeadLocation)
{
	if (IsAllPlayerDead())
	{
		ASSGameState* SSGameState = GetGameState<ASSGameState>();
		int32 CurPlayerRespawnRemain = SSGameState->GetRemainPlayerRespawnCount();
		if (CurPlayerRespawnRemain > 0)
		{
			FTimerHandle RespawnTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				RespawnTimerHandle,
				FTimerDelegate::CreateLambda([&]() {
					RespawnPlayers(DeadLocation);
					}),
				1.0f,
				false,
				5.0f);
		}
		else
		{
			EndServer(false);
		}
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

void ASSGameMode::EndServer(bool bCleared)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController)
		{
			ASSPlayerController* SSPlayerController = CastChecked<ASSPlayerController>(PlayerController);
			ASSPlayerState* SSPlayerState = SSPlayerController->GetPlayerState<ASSPlayerState>();

			if (IsValid(SSPlayerController) && IsValid(SSPlayerState))
			{
				// 플레이어 통계 & 게임 결과 출력을 위해 Client RPC 호출
				SSPlayerController->ClientRpcSetAndShowFinalGameStatistics(bCleared, SSPlayerState->GetPlayStatistics());
			}
		}
	}

	// Server 리셋을 위해 준비 중임을 표시
	bWaitingForResetServer = true;
	StopServer();
}

void ASSGameMode::StopServer()
{
	// AI 동작을 멈추고, 몬스터가 더 스폰되지 않게 설정
	SetNonPlayerCharacterStopAI();
	SetNonPlayerCharacterSpawn(false);	
}

void ASSGameMode::ResetServer()
{
	SS_LOG(LogSSNetwork, Log, TEXT("ResetServer"))

	ASSGameState* SSGameState = GetGameState<ASSGameState>();
	SSGameState->SetTotalKilledMonsterCount(0);
	SSGameState->SetRemainPlayerRespawnCount(PlayerRespawnLimits);

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

const int32 ASSGameMode::GetPlayerIndex(APlayerController* QueryPlayerController)
{
	int32 RetIndex = 0;
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController == QueryPlayerController)
		{
			return RetIndex;
		}
		++RetIndex;
	}
	return -1;
}

const int32 ASSGameMode::ClampPlayerIndex(int32 CurIndex)
{
	return FMath::Clamp(CurIndex, 0, CurPlayerNum - 1);
}
