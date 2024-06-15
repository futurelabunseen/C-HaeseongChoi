// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataPrecisionStrike.h"
#include "Strata/SSStrataRocket.h"

USSStrataPrecisionStrike::USSStrataPrecisionStrike()
{
	DelayTime = 5.0f;
	CoolTime = 20.0f;
	StrataType = EStrataType::OFFENSE;

	CommandArray = TArray<EStrataCommand>{ EStrataCommand::RIGHT, EStrataCommand::RIGHT, EStrataCommand::UP };
}

void USSStrataPrecisionStrike::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector& TargetLocation)
{
	Super::ActivateStratagem(CurWorld, StrataCauser, TargetLocation);

	if (!IsValid(CurWorld)) return;

	// 미사일 스폰
	FString PrecisionStrikePath = TEXT("/Game/SuperSoldier/Props/BP_PrecisionStrike.BP_PrecisionStrike_C");
	UClass* PrecisionStrikeClass = StaticLoadClass(UObject::StaticClass(), nullptr, *PrecisionStrikePath);
	if (PrecisionStrikeClass)
	{
		ASSStrataRocket* PrecisionStrikeActor = CurWorld->SpawnActor<ASSStrataRocket>(PrecisionStrikeClass);
		PrecisionStrikeActor->SetStrataCauser(StrataCauser);

		FVector PrecisionStrikeLocation = TargetLocation;
		PrecisionStrikeLocation.Z += 4000.0f;
		PrecisionStrikeActor->SetActorLocation(PrecisionStrikeLocation);

		UFunction* StrikeFunction = PrecisionStrikeClass->FindFunctionByName(FName(TEXT("Strike")));
		if (StrikeFunction)
		{
			FVector StrikeLocation = TargetLocation;
			PrecisionStrikeActor->Strike(StrikeLocation);
		}
	}
}
