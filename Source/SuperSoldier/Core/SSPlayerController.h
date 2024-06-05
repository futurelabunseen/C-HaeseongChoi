// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

// UI Widget Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USSUserPlayWidget> UserPlayWidgetClass;

	UPROPERTY()
	TObjectPtr<class USSUserPlayWidget> UserPlayWidget;
};
