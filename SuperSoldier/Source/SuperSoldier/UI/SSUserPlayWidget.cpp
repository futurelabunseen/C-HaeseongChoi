// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SSUserPlayWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

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
}

void USSUserPlayWidget::UpdateHPBar(float NewCurrentHP)
{
	ensure(MaxHP > 0.0f);

	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(NewCurrentHP / MaxHP);
	}
}
