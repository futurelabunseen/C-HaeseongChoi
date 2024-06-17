// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SSUserResultWidget.h"
#include "Components/TextBlock.h"

USSUserResultWidget::USSUserResultWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USSUserResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KillCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextKillCount")));
	ensure(KillCountText);

	TeamKillCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextTeamKillCount")));
	ensure(TeamKillCountText);

	DeathCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextDeathCount")));
	ensure(DeathCountText);

	ReviveCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextReviveCount")));
	ensure(ReviveCountText);

	UsedStratagemNumText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextUsedStratagemNum")));
	ensure(UsedStratagemNumText);

	GameResultText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextGameResult")));
	ensure(GameResultText);
}

void USSUserResultWidget::UpdateKillCount(int32 NewKillCount)
{
	KillCountText->SetText(FText::FromString(FString::FromInt(NewKillCount)));
}

void USSUserResultWidget::UpdateTeamKillCount(int32 NewTeamKillCount)
{
	TeamKillCountText->SetText(FText::FromString(FString::FromInt(NewTeamKillCount)));
}

void USSUserResultWidget::UpdateDeathCount(int32 NewDeathCount)
{
	DeathCountText->SetText(FText::FromString(FString::FromInt(NewDeathCount)));
}

void USSUserResultWidget::UpdateReviveCount(int32 NewReviveCount)
{
	ReviveCountText->SetText(FText::FromString(FString::FromInt(NewReviveCount)));
}

void USSUserResultWidget::UpdateUsedStratagemNum(int32 NewUsedStratagemNum)
{
	UsedStratagemNumText->SetText(FText::FromString(FString::FromInt(NewUsedStratagemNum)));
}

void USSUserResultWidget::UpdateGameResultText(bool bCleared)
{
	if (bCleared)
	{
		GameResultText->SetText(FText::FromString(TEXT("Victory!!!")));
	}
	else
	{
		GameResultText->SetText(FText::FromString(TEXT("Defeat...")));
	}
}