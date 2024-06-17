// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SSUserPlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSUserPlayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USSUserPlayWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHP(float NewMaxHP) { MaxHP = NewMaxHP; }

	UFUNCTION()
	void UpdateHPBar(float NewCurrentHP);

	UFUNCTION()
	void UpdateBloodyEffect(float NewCurrentHP);

	UFUNCTION()
	void UpdateCrossHair(bool bAiming);

	UFUNCTION()
	void UpdateTotalKillCount(int32 TotalKilledMonsterCount);

	UFUNCTION()
	void ShowStratagemList(bool bCalling);

	UFUNCTION()
	void FadeInOut(bool bFadeOut);
protected:
	UPROPERTY()
	TObjectPtr<class UImage> CrossHairImage;

	UPROPERTY()
	TObjectPtr<class UProgressBar> HPProgressBar;
	
	UPROPERTY()
	float MaxHP;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UWidgetAnimation> AnimShowStrataList;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TotalKillCountText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UWidgetAnimation> AnimFadeInOut;

	UPROPERTY()
	TObjectPtr<class UImage> BloodyEffectImage;
};
