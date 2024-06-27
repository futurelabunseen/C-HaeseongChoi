// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataGameStart.h"
#include "Core/SSGameMode.h"

USSStrataGameStart::USSStrataGameStart()
{
	DelayTime = 4.0f;
	CoolTime = 0.0f;
	StrataType = EStrataType::CORE;

	CommandArray = TArray<EStrataCommand>{ EStrataCommand::UP, EStrataCommand::UP, EStrataCommand::DOWN, EStrataCommand::DOWN };

}

void USSStrataGameStart::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector& TargetLocation)
{
	if (!IsValid(CurWorld)) return;

	ASSGameMode* SSGameMode = Cast<ASSGameMode>(CurWorld->GetAuthGameMode());
	if (IsValid(SSGameMode))
	{
		SSGameMode->GameStart();
	}
}
