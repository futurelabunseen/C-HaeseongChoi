// Fill out your copyright notice in the Description page of Project Settings.


#include "Strata/SSStrataAirStrike.h"
#include "Strata/SSStrataRocket.h"

USSStrataAirStrike::USSStrataAirStrike()
{
	DelayTime = 2.5f;
	CoolTime = 25.0f;
	StrataType = EStrataType::OFFENSE;

	CommandArray = TArray<EStrataCommand>{ 
		EStrataCommand::UP, 
		EStrataCommand::RIGHT, 
		EStrataCommand::DOWN, 
		EStrataCommand::RIGHT };
}

void USSStrataAirStrike::ActivateStratagem(UWorld* const CurWorld, AController* const StrataCauser, const FVector TargetLocation, const FVector ThrowedDirection)
{
	Super::ActivateStratagem(CurWorld, StrataCauser, TargetLocation, ThrowedDirection);

	if (!IsValid(CurWorld)) return;

	// 던진 방향의 XY 정보만 사용
	FVector ThrowedDirectionXY = { ThrowedDirection.X, ThrowedDirection.Y, 0.0f };
	ThrowedDirectionXY.Normalize();

	// 외적을 통해 던져진 방향에 수직이 되는 벡터를 구한다.
	FVector PerpendicularVector = ThrowedDirectionXY ^ FVector::UpVector;
	PerpendicularVector.Normalize();

	// 미사일 간격과 개수
	const float Spacing = 800.0f;
	const int SpawnStrikeNum = 5;

	// 타격될 위치를 설정
	TArray<FVector> StrikePosition;
	StrikePosition.Add(TargetLocation - PerpendicularVector * Spacing * (SpawnStrikeNum / 2));
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
			// 비스듬하게 떨어지도록 위치를 설정
			FVector SpawnLocation = StrikePosition[i] - PerpendicularVector * Spacing;
			SpawnLocation.Z += 4000.0f + 850.0f * i;
			AirStrikeActor->SetActorLocation(SpawnLocation);

			// 타격 위치로 날아가도록 함수 호출
			AirStrikeActor->Strike(StrikePosition[i]);
		}
	}
}
