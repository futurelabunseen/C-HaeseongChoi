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
	// CharacterControl Section
	virtual void SetCharacterControlData(const class USSCharacterControlData* CharacterControlData);

	TMap<ECharacterControlType, class USSCharacterControlData*> CharacterControlManager;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	bool AttemptSprint();
	void Sprint(const FInputActionValue& Value);
// Aim Section
protected:
	bool bAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	void Aim(const FInputActionValue& Value);

// Fire & Throw Section
protected:
	bool bFiring;
	bool bThrowing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ThrowMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ThrowAction;

	void Fire(const FInputActionValue& Value);
	void EndFire(UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void Throw(const FInputActionValue& Value);
	void EndThrow(UAnimMontage* TargetMontage, bool IsProperlyEnded);
// Strata Section
protected:
	bool bCalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> CallMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CallAction;

	void Call(const FInputActionValue& Value);
// Crosshair Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UUserWidget> CrosshairWidget;

// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;

};
