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

	static ConstructorHelpers::FObjectFinder<UCurveFloat> TurnInPlaceCurveFloatRef(
		TEXT("/Game/SuperSoldier/Curves/TurnInPlaceCurve.TurnInPlaceCurve"));
	if (TurnInPlaceCurveFloatRef.Object)
	{
		TurnInPlaceCurveFloat = TurnInPlaceCurveFloatRef.Object;
	}

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
	if (TurnInPlaceCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ASSCharacterNonPlayer::UpdateTurnInPlaceProgress);
		TurnInPlaceTimeline.AddInterpFloat(TurnInPlaceCurveFloat, TimelineProgress);
	}
}

void ASSCharacterNonPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TurnInPlaceTimeline.TickTimeline(DeltaSeconds);
}

void ASSCharacterNonPlayer::SetDead()
{
	Super::SetDead();
	
	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ASSCharacterNonPlayer::Dissolve, DissolveDelayTime, false);
}

void ASSCharacterNonPlayer::Attack()
{
	const float AnimationSpeedRate = 1.5f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackMontage, AnimationSpeedRate);

	NetMulticastRpcShowAttackAnimation();

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

void ASSCharacterNonPlayer::NetMulticastRpcShowAttackAnimation_Implementation()
{
	const float AnimationSpeedRate = 1.5f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackMontage, AnimationSpeedRate);
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
	const float AnimationSpeedRate = 2.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(TurnInPlaceMontage, AnimationSpeedRate);

	if (bTurnRight)
	{
		AnimInstance->Montage_JumpToSection(TEXT("TurnRight"), TurnInPlaceMontage);
	}

	TurnInPlaceBeginRotation = GetActorRotation();

	SetActorTickEnabled(true);
	TurnInPlaceTimeline.PlayFromStart();
}

void ASSCharacterNonPlayer::UpdateTurnInPlaceProgress(const float Value)
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

			const float LerpT = FMath::Clamp(Value, 0.0f, 1.0f);
			FRotator NewRotator = FMath::Lerp(TurnInPlaceBeginRotation, ToTargetRot, LerpT);

			SetActorRotation(NewRotator);
		}
	}

	if (Value >= 1.f)
	{
		SetActorTickEnabled(false);
		OnActionFinished.ExecuteIfBound();
	}
}
