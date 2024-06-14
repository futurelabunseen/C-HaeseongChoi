// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SSCharacterPlayer.h"
#include "SS_MurdockPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCallingDelegate, bool);

enum class ECharacterControlType : uint8
{
	Normal,
	Aiming
};

/**
 * 
 */
UCLASS()
class SUPERSOLDIER_API ASS_MurdockPlayer : public ASSCharacterPlayer
{
	GENERATED_BODY()
public:
	ASS_MurdockPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	void ResetPlayerInputVariable();
	virtual void BeginPlay() override;

// CharacterControl Section
protected:
	TMap<ECharacterControlType, class USSCharacterControlData*> CharacterControlManager;

	virtual void SetCharacterControlData(const class USSCharacterControlData* CharacterControlData);

// Initialize Section
protected:
	void InitializeStratagem();
	void InitializeWeapon();

// Look Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	void Look(const FInputActionValue& Value);

// Sprint Section
protected:
	bool bSprintKeyPressing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	void AttemptSprint();
	void Sprint(const FInputActionValue& Value);
	void SetSprintToMovementComponent(bool bNewSprint);
	void AttemptSprintEndDelegate(UAnimMontage* TargetMontage, bool IsProperlyEnded);

// Aim Section
protected:
	bool bAiming;
	FOnAimingDelegate OnAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	void Aim(const FInputActionValue& Value);
	void SetAimingToMovementComponent(bool bNewAiming);

// Fire Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TObjectPtr<class USSWeaponComponent> MainWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	void Fire(const FInputActionValue& Value);

// Attack Hit Section
protected:
	virtual void AttackHitCheck(FName AttackId = TEXT("None")) override;
	virtual void PlaySoundEffect() override;

// Throw Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ThrowMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ThrowAction;

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

	TObjectPtr<class ASSStrataIndicator> CurStrataIndicator;

	void Call(const FInputActionValue& Value);
	void EndCalling(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void EndStrata(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void TranslateInput(const FInputActionValue& Value);
	bool MatchingInput();
	void ProcessCommandInput(const FInputActionValue& Value);
	virtual void ReleaseThrowable() override;

// Take Damage Section
public:
	virtual void SetDead();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

// Widget Section
public:
	virtual void SetupCharacterWidget(class USSUserPlayWidget* InUserWidget) override;

// Respawn Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> RespawnMontage;

	virtual void Respawn(const FVector& TargetLocation) override;

// Rpc Section
protected:
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

	virtual void NetMulticastRpcShowAnimationMontage_Implementation(UAnimMontage* MontageToPlay, const float AnimationSpeedRate) override;
};
