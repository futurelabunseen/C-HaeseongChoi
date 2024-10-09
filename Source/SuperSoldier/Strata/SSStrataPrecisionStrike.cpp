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

void USSStrataPrecisionStrike::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector TargetLocation, const FVector ThrowedDirection)
{
	Super::ActivateStratagem(CurWorld, StrataCauser, TargetLocation, ThrowedDirection);

	if (!IsValid(CurWorld)) return;

	// 미사일 스폰
	FString PrecisionStrikePath = TEXT("/Game/SuperSoldier/Props/BP_PrecisionStrike.BP_PrecisionStrike_C");
	UClass* PrecisionStrikeClass = StaticLoadClass(UObject::StaticClass(), nullptr, *PrecisionStrikePath);

	if (PrecisionStrikeClass)
	{
		ASSStrataRocket* PrecisionStrikeActor = CurWorld->SpawnActor<ASSStrataRocket>(PrecisionStrikeClass);
		PrecisionStrikeActor->SetStrataCauser(StrataCauser);

		// 미사일 위치를 설정
		FVector PrecisionStrikeLocation = TargetLocation;
		const float SpawnOffset = 4000.0f;
		PrecisionStrikeLocation.Z += SpawnOffset;
		PrecisionStrikeActor->SetActorLocation(PrecisionStrikeLocation);

		// TargetLocation으로 미사일을 날린다
		FVector StrikeLocation = TargetLocation;
		PrecisionStrikeActor->Strike(StrikeLocation);
	}
}
