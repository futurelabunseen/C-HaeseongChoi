// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SSUserPlayWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

USSUserPlayWidget::USSUserPlayWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USSUserPlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHPBar")));
	ensure(HPProgressBar);

	CrossHairImage = Cast<UImage>(GetWidgetFromName(TEXT("ImgCrossHair")));
	ensure(CrossHairImage);

	CrossHairImage->SetVisibility(ESlateVisibility::Hidden);

	TotalKillCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextTotalKillCount")));
	ensure(TotalKillCountText);

	RemainPlayerSpawnCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextRemainPlayerSpawnCount")));
	ensure(RemainPlayerSpawnCountText);

	BloodyEffectImage = Cast<UImage>(GetWidgetFromName(TEXT("ImgBloodyEffect")));
	ensure(BloodyEffectImage);

	PrevBroadcastedHP = 0.0f;
	bShowToggleKeyInfo = true;
}

void USSUserPlayWidget::UpdateHPBar(float NewCurrentHP)
{
	ensure(MaxHP > 0.0f);

	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(NewCurrentHP / MaxHP);
	}
}

void USSUserPlayWidget::UpdateBloodyEffect(float NewCurrentHP)
{
	ensure(MaxHP > 0.0f);
	if (PrevBroadcastedHP > NewCurrentHP && BloodyEffectImage)
	{
		PlayBloodyEffect();
	}
	PrevBroadcastedHP = NewCurrentHP;
}

void USSUserPlayWidget::UpdateCrossHair(bool bAiming)
{
	if (bAiming)
	{
		CrossHairImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CrossHairImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USSUserPlayWidget::UpdateTotalKillCount(int32 TotalKilledMonsterCount)
{
	TotalKillCountText->SetText(FText::FromString(FString::FromInt(TotalKilledMonsterCount)));
}

void USSUserPlayWidget::UpdateRemainPlayerSpawnCount(int32 RemainPlayerSpawnCount)
{
	RemainPlayerSpawnCountText->SetText(FText::FromString(FString::FromInt(RemainPlayerSpawnCount)));
}

void USSUserPlayWidget::ShowStratagemList(bool bCalling)
{
	if (bCalling)
	{
		PlayAnimation(AnimShowStrataList);
	}
	else
	{
		PlayAnimation(AnimShowStrataList, 0.0f, 1, EUMGSequencePlayMode::Reverse);
	}
}

void USSUserPlayWidget::FadeInOut(bool bFadeOut)
{
	if (bFadeOut)
	{
		PlayAnimation(AnimFadeInOut);
	}
	else
	{
		PlayAnimation(AnimFadeInOut, 0.0f, 1, EUMGSequencePlayMode::Reverse);
	}
}

void USSUserPlayWidget::PlayBloodyEffect()
{
	if (AnimBloodyEffect)
	{
		PlayAnimation(AnimBloodyEffect);
	}
}

void USSUserPlayWidget::ToggleKeyInfo()
{
	if (bShowToggleKeyInfo)
	{
		PlayAnimation(ToggleKeyInfoAnimation);
	}
	else
	{
		PlayAnimation(ToggleKeyInfoAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse);
	}

	bShowToggleKeyInfo = !bShowToggleKeyInfo;
}
