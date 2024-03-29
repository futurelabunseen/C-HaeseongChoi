// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStratagem.h"

USSStratagem::USSStratagem()
{
	ArrCommand.Add(EStrataCommand::UP);
	ArrCommand.Add(EStrataCommand::RIGHT);
	ArrCommand.Add(EStrataCommand::DOWN);
	ArrCommand.Add(EStrataCommand::LEFT);
}

USSStratagem::~USSStratagem()
{
}

const TArray<EStrataCommand>& USSStratagem::GetCommandSequence() const
{
	return ArrCommand;
}

void USSStratagem::ActivateStratagem()
{
	UE_LOG(LogTemp, Log, TEXT("ActivateStratagem"))
}
