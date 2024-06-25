// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataAirStrike.h"
#include "Strata/SSStrataRocket.h"

USSStrataAirStrike::USSStrataAirStrike()
{
	DelayTime = 2.5f;
	CoolTime = 25.0f;
	StrataType = EStrataType::OFFENSE;

	CommandArray = TArray<EStrataCommand>{ EStrataCommand::UP, EStrataCommand::RIGHT, EStrataCommand::DOWN, EStrataCommand::RIGHT };
}

void USSStrataAirStrike::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector& TargetLocation)
{
	Super::ActivateStratagem(CurWorld, StrataCauser, TargetLocation);

	if (!IsValid(CurWorld)) return;

	// 던진 방향의 XY 정보만 사용
	ThrowedDirection.Z = 0.0f;
	ThrowedDirection.Normalize();

	// 외적을 통해 던져진 방향에 수직이 되는 벡터를 구한다.
	FVector PerpendicularVector = ThrowedDirection ^ FVector::UpVector;
	if (PerpendicularVector.IsNearlyZero())
	{
		PerpendicularVector = ThrowedDirection ^ FVector::RightVector;
	}
	PerpendicularVector.Normalize();

	// 미사일 간격과 개수
	const float Spacing = 800.0f;
	const int SpawnStrikeNum = 5;

	// 스폰될 위치를 설정
	TArray<FVector> StrikePosition;
	StrikePosition.Add(TargetLocation - PerpendicularVector * Spacing * (SpawnStrikeNum / 2 - 1));
	for (int i = 0; i < SpawnStrikeNum - 1; ++i)
	{
		StrikePosition.Add(StrikePosition.Last() + PerpendicularVector * Spacing);
	}

	FString AirStrikePath = TEXT("/Game/SuperSoldier/Props/BP_AirStrike.BP_AirStrike_C");
	UClass* AirStrikeClass = StaticLoadClass(UObject::StaticClass(), nullptr, *AirStrikePath);
	if (AirStrikeClass)
	{
		for (int i = 0; i < SpawnStrikeNum; ++i)
		{
			ASSStrataRocket* AirStrikeActor = CurWorld->SpawnActor<ASSStrataRocket>(AirStrikeClass);
			AirStrikeActor->SetStrataCauser(StrataCauser);

			// 순차적으로 떨어지도록 Z값을 설정
			FVector AirStrikeLocation = StrikePosition[i];
			AirStrikeLocation.Z += 4000.0f + 850.0f * i;
			AirStrikeActor->SetActorLocation(AirStrikeLocation);

			// 타격 위치는 스트라타젬이 던져진 방향에 수직인 벡터를 이용해 설정
			FVector StrikeLocation = StrikePosition[i] - PerpendicularVector * 800;
			AirStrikeActor->Strike(StrikeLocation);
		}
	}
}
