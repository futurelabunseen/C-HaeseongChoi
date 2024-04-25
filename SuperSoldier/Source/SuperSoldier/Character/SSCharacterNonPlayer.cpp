// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterNonPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ASSCharacterNonPlayer::ASSCharacterNonPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(200.f, 200.0f);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Mesh & AnimInstance
	GetMesh()->SetRelativeLocationAndRotation(FVector(-10.0f, 0.0f, -199.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

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

	// AIController
	AIControllerClass = ASSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bTurnInPlace = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> TurnInPlaceMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Animations/AM_KrakenTurn.AM_KrakenTurn"));
	if (TurnInPlaceMontageRef.Object)
	{
		TurnInPlaceMontage = TurnInPlaceMontageRef.Object;
	}
}

void ASSCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ASSCharacterNonPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
				FRotator NewRotator = FMath::RInterpTo(GetActorRotation(), ToTargetRot, DeltaSeconds, 2.0f);

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

void ASSCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	ASSAIController* AIController = Cast<ASSAIController>(GetController());
	if (AIController)
	{
		AIController->StopAI();
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ASSCharacterNonPlayer::Dissolve, DissolveDelayTime, false);
}

void ASSCharacterNonPlayer::NetMulticastRpcShowAnimationMontage_Implementation(UAnimMontage* MontageToPlay, FName SectionName, const float AnimationSpeedRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(MontageToPlay, AnimationSpeedRate);

	if (SectionName != TEXT(""))
	{
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
	}
}

void ASSCharacterNonPlayer::Attack()
{
	const float AnimationSpeedRate = 1.5f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackMontage, AnimationSpeedRate);

	NetMulticastRpcShowAnimationMontage(AttackMontage, TEXT(""), AnimationSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ASSCharacterNonPlayer::NotifyActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void ASSCharacterNonPlayer::AttackHitCheck()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("AttackHitCheck"))
	}
}

void ASSCharacterNonPlayer::NotifyActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	OnActionFinished.ExecuteIfBound();
}

void ASSCharacterNonPlayer::SetAIActionDelegate(const FAICharacterActionFinished& InOnActionFinished)
{
	OnActionFinished = InOnActionFinished;
}

void ASSCharacterNonPlayer::TurnInPlace(bool bTurnRight)
{
	bTurnInPlace = true;
	SetActorTickEnabled(true);

	const float AnimationSpeedRate = 2.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(TurnInPlaceMontage, AnimationSpeedRate);

	if (bTurnRight)
	{
		AnimInstance->Montage_JumpToSection(TEXT("TurnRight"), TurnInPlaceMontage);
		NetMulticastRpcShowAnimationMontage(AttackMontage, TEXT("TurnRight"), AnimationSpeedRate);
	}

	else
	{
		NetMulticastRpcShowAnimationMontage(AttackMontage, TEXT(""), AnimationSpeedRate);
	}
}
