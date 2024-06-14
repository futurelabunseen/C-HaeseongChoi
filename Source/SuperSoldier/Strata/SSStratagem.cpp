// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStratagem.h"

USSStratagem::USSStratagem()
{
	DelayTime = 0.0f;
}

const TArray<EStrataCommand>& USSStratagem::GetCommandSequence() const
{
	return CommandArray;
}

void USSStratagem::ActivateStratagem(UWorld* const CurWorld, AActor* const StrataCauser, const FVector& TargetLocation)
{
}
