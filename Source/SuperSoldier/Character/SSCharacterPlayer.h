// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterBase.h"
#include "InputActionValue.h"
#include "Strata/SSStratagem.h"
#include "SSCharacterPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCallingDelegate, bool);

/**
 * 
 */

UCLASS()
class SUPERSOLDIER_API ASSCharacterPlayer : public ASSCharacterBase
{
	GENERATED_BODY()
public:
	ASSCharacterPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	// CharacterControl Section
	virtual void SetCharacterControlData(const class USSCharacterControlData* CharacterControlData);

	TMap<ECharacterControlType, class USSCharacterControlData*> CharacterControlManager;
protected:
	virtual void BeginPlay() override;
	bool GetAnyMontagePlaying(UAnimMontage* FilterMontage = NULL);
	void AttemptSprintEndDelegate(UAnimMontage* TargetMontage, bool IsProperlyEnded);

// Camera Section
protected:
	// 지지대 역할을 하는 스프링암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float YawDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float PitchDelta;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> NormalInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Unreliable)
	void ServerRpcSetYawPitchDelta(float NewYawDelta, float NewPitchDelta);

	UFUNCTION(Client, Unreliable)
	void ClientRpcSetYawPitchDelta(ASSCharacterPlayer* CharacterToSet, float NewYawDelta, float NewPitchDelta);
// Sprint Section
protected:
	bool bSprintKeyPressing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	void AttemptSprint();
	void Sprint(const FInputActionValue& Value);
	void SetSprintToMovementComponent(bool bNewSprint);

// Aim Section
protected:
	bool bAiming;
	FOnAimingDelegate OnAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	void Aim(const FInputActionValue& Value);
	void SetAimingToMovementComponent(bool bNewAiming);

// Fire & Throw Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TObjectPtr<class USSWeaponComponent> MainWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ThrowMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ThrowAction;

	void Fire(const FInputActionValue& Value);
	void Throw(const FInputActionValue& Value);

// Strata Section
protected:
	FOnCallingDelegate OnCalling;

	bool bCalling;
	bool bReadyForThrowingStrata;
	bool bChangeMontageForThrowingStrata;

	int32 SelectedStrataIndex;
	TArray<EStrataCommand> InputSequence;
	TArray<std::pair<FName, USSStratagem*>> AvailableStratagems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> CallMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> StrataReadyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> StrataThrowMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CallAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CommandAction;

	void Call(const FInputActionValue& Value);
	void EndCalling(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void EndStrata(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void TranslateInput(const FInputActionValue& Value);
	bool MatchingInput();
	void ProcessCommandInput(const FInputActionValue& Value);

// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;
	virtual void PlaySoundEffect() override;

// Throw Section
public:
	virtual void ReleaseThrowable() override;
	TObjectPtr<class ASSStrataIndicator> CurStrataIndicator;

// HitReact Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> HitReactMontage;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

// Widget Section
public:
	virtual void SetupCharacterWidget(class USSUserPlayWidget* InUserWidget);

// Respawn Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> RespawnMontage;

	virtual void Respawn(const FVector& TargetLocation);

// RPC Section
protected:
	void RpcPlayAnimation(UAnimMontage* MontageToPlay);
	void RpcJumpToSection(UAnimMontage* MontageToPlay, FName SectionName);

	UFUNCTION(Client, Unreliable)
	void ClientRpcPlayAnimation(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay);

	UFUNCTION(Client, Unreliable)
	void ClientRpcJumpToSection(ASSCharacterPlayer* CharacterToPlay, UAnimMontage* MontageToPlay, FName SectionName);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcNotifyFireHit(const FHitResult& HitResult);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcThrow();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcCalling();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcEndCalling();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcStrataReady(const FName& StratagemName);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRpcStrataThrow();

	virtual void Tick(float DeltaSeconds) override;
};
