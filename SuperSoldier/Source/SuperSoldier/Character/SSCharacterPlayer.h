// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "InputActionValue.h"
#include "SSCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASSCharacterPlayer : public ASSCharacterBase
{
	GENERATED_BODY()
public:
	ASSCharacterPlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

// Camera Section
protected:
	// 지지대 역할을 하는 스프링암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> NormalInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

// Sprint Section
protected:
	bool bSprint;
	FTimerHandle SpeedChangeTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	void Sprint(const FInputActionValue& Value);
	void UpdateMaxWalkSpeed();

// Aim Section
protected:
	bool bAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	void Aim(const FInputActionValue& Value);

// Fire Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	void Fire(const FInputActionValue& Value);
};
