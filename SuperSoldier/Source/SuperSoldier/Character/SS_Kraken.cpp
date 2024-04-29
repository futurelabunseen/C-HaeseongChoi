// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_Kraken.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSKrakenAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ASS_Kraken::ASS_Kraken(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Meshes/KRAKEN.KRAKEN"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Animations/ABP_Kraken.ABP_Kraken_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Animations/AM_KrakenAttack.AM_KrakenAttack"));
	if (AttackMontageRef.Object)
	{
		AttackMontage = AttackMontageRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TurnInPlaceMontageRef(
			TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Animations/AM_KrakenTurn.AM_KrakenTurn"));
	if (TurnInPlaceMontageRef.Object)
	{
		TurnInPlaceMontage = TurnInPlaceMontageRef.Object;
	}

	bTurnInPlace = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(200.f, 200.0f);

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// Mesh & AnimInstance
	GetMesh()->SetRelativeLocationAndRotation(FVector(-10.0f, 0.0f, -199.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

	// AI
	AIControllerClass = ASSKrakenAIController::StaticClass();
}

void ASS_Kraken::Tick(float DeltaSeconds)
{
	if (bTurnInPlace && HasAuthority())
	{
		AAIController* CustomController = Cast<AAIController>(GetController());
		if (CustomController)
		{
			UObject* TargetPlayerPtr = CustomController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer"));
			APawn* TargetPawn = Cast<APawn>(TargetPlayerPtr);

			if (TargetPawn)
			{
				FVector ToTargetVec = TargetPawn->GetActorLocation() - GetActorLocation();
				ToTargetVec.Z = 0.0f;

				FRotator ToTargetRot = FRotationMatrix::MakeFromX(ToTargetVec).Rotator();
				FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), ToTargetRot, DeltaSeconds, 4.0f);

				SetActorRotation(NewRotator);

				double DeltaYaw = ToTargetRot.Yaw - GetActorRotation().Yaw;
				if (FMath::Abs(DeltaYaw) < 20.0f)
				{
					bTurnInPlace = false;
					SetActorTickEnabled(false);
					OnActionFinished.ExecuteIfBound();
				}
			}
		}
	}
}

void ASS_Kraken::TurnInPlace(bool bTurnRight)
{
	Super::TurnInPlace(bTurnRight);

	bTurnInPlace = true;
	SetActorTickEnabled(true);

	const float AnimationSpeedRate = 2.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(TurnInPlaceMontage, AnimationSpeedRate);

	if (bTurnRight)
	{
		AnimInstance->Montage_JumpToSection(TEXT("TurnRight"), TurnInPlaceMontage);
		NetMulticastRpcShowAnimationMontage(TurnInPlaceMontage, TEXT("TurnRight"), AnimationSpeedRate);
	}

	else
	{
		NetMulticastRpcShowAnimationMontage(TurnInPlaceMontage, TEXT(""), AnimationSpeedRate);
	}
}
