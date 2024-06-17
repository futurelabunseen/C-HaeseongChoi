// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SSUserResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API USSUserResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USSUserResultWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void UpdateKillCount(int32 NewKillCount);

	UFUNCTION()
	void UpdateTeamKillCount(int32 NewTeamKillCount);

	UFUNCTION()
	void UpdateDeathCount(int32 NewDeathCount);

	UFUNCTION()
	void UpdateReviveCount(int32 NewReviveCount);

	UFUNCTION()
	void UpdateUsedStratagemNum(int32 NewUsedStratagemNum);

	UFUNCTION()
	void UpdateGameResultText(bool bCleared);
protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> KillCountText;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TeamKillCountText;

	UPROPERTY()
	TObjectPtr<class UTextBlock> DeathCountText;

	UPROPERTY()
	TObjectPtr<class UTextBlock> ReviveCountText;

	UPROPERTY()
	TObjectPtr<class UTextBlock> UsedStratagemNumText;

	UPROPERTY()
	TObjectPtr<class UTextBlock> GameResultText;
};
