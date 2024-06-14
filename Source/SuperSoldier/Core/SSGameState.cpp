// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SSGameState.h"
#include "SuperSoldier.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Core/SSPlayerController.h"

void ASSGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();
}

void ASSGameState::OnRep_ReplicatedHasBegunPlay()
{
	Super::OnRep_ReplicatedHasBegunPlay();
}

void ASSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASSGameState, TotalKilledMonsterCount);
}

void ASSGameState::OnRep_TotalKilledMonsterCount()
{
	OnTotalKilledMonsterCountChangedDelegate.Broadcast(TotalKilledMonsterCount);
}

void ASSGameState::NetMulticast_GameClear_Implementation()
{
	if (HasAuthority()) return;

	ASSPlayerController* SSLocalPlayerController = Cast<ASSPlayerController>(GetWorld()->GetFirstPlayerController());
	SSLocalPlayerController->ShowGameResult(true);
}
