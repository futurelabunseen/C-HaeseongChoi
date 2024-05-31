// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SStrataReinforcements.h"

USStrataReinforcements::USStrataReinforcements()
{
	DelayTime = 5.0f;
	CoolTime = 20.0f;
	StrataType = EStrataType::SUPPORT;

	CommandArray = TArray<EStrataCommand>{ 
		EStrataCommand::UP, 
		EStrataCommand::DOWN, 
		EStrataCommand::RIGHT, 
		EStrataCommand::LEFT,
		EStrataCommand::UP };
}

void USStrataReinforcements::ActivateStratagem(UWorld* const CurWorld, const FVector& TargetLocation)
{
	// Revive All Dead Player
	UE_LOG(LogTemp, Log, TEXT("USStrataReinforcements - Revive All Dead Player"));
}
