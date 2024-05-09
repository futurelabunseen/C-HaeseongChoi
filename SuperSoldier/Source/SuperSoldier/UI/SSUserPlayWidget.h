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

protected:
	UPROPERTY()
	TObjectPtr<class UImage> CrossHairImage;

	UPROPERTY()
	TObjectPtr<class UProgressBar> HPProgressBar;
	
	UPROPERTY()
	float MaxHP;
};
