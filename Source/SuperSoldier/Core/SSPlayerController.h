// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SSPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPossessionChanged, APawn*);
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

	virtual void OnPossess(APawn* aPawn) override;
	virtual void AcknowledgePossession(class APawn* P) override;

// UI Play Widget Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USSUserPlayWidget> UserPlayWidgetClass;

	UPROPERTY()
	TObjectPtr<class USSUserPlayWidget> UserPlayWidget;

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

// IMC Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> ControllerInputMappingContext;

// Spectate Section
public:
	FOnPossessionChanged OnPossessionChanged;

	virtual void SetupInputComponent() override;

	UFUNCTION(Server, Unreliable)
	void ServerRpcSpectatePrevious();

	UFUNCTION(Server, Unreliable)
	void ServerRpcSpectateNext();

	UFUNCTION()
	void UpdateViewTarget(APawn* aPawn);

	UFUNCTION()
	void ClearPrevDelegate();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SpectatePreviousAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SpectateNextAction;

	int32 SpectateOffset;
	int IndividualPlayerIndex;

	void SpectatePrevious(const FInputActionValue& Value);
	void SpectateNext(const FInputActionValue& Value);

	void UpdatePlayerIndex();
	void SpectateTarget();

// Toggle KeyInfo Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ToggleKeyInfoAction;

	void ToggleKeyInfo(const FInputActionValue& Value);
};
