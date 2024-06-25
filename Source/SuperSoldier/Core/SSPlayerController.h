// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ASSPlayerController();
	virtual void BeginPlay() override;

protected:
	virtual void PostInitializeComponents() override;
	virtual void PostNetInit() override;

	virtual void AcknowledgePossession(class APawn* P) override;

// UI Play Widget Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USSUserPlayWidget> UserPlayWidgetClass;

	UPROPERTY()
	TObjectPtr<class USSUserPlayWidget> UserPlayWidget;

// Respawn Camera Lerp Section
public:
	UFUNCTION(Client, Unreliable)
	void ClientRpcBlendViewTargetToNewPawn(APawn* NewPawn);

// Show GameResult
public:
	void ShowGameResult(bool bCleared);

	UFUNCTION(Client, Reliable)
	void ClientRpcSetAndShowFinalGameStatistics(bool bCleared, const struct FPlayStatistics& FinalPlayStatistics);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USSUserResultWidget> UserResultWidgetClass;

	UPROPERTY()
	TObjectPtr<class USSUserResultWidget> UserResultWidget;

// Spectate Section
public:
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> SpectateInputMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SpectatePreviousAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SpectateNextAction;

	void SpectatePrevious(const FInputActionValue& Value);
	void SpectateNext(const FInputActionValue& Value);
};
