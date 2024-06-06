// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SS_Kraken.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/SSKrakenAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Physics/SSColision.h"
#include "Engine/DamageEvents.h"
#include "Character/SSCharacterPlayer.h"

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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> HitReactMontageRef(
		TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Animations/AM_KrakenHit.AM_KrakenHit"));
	if (HitReactMontageRef.Object)
	{
		HitReactMontage = HitReactMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> MoanSoundRef(TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Sounds/SC_KrakenHitMoan.SC_KrakenHitMoan"));
	if (MoanSoundRef.Object)
	{
		MoanSound = MoanSoundRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> DeadSoundRef(TEXT("/Game/SuperSoldier/Characters/Monsters/Kraken/Sounds/SC_KrakenDead.SC_KrakenDead"));
	if (DeadSoundRef.Object)
	{
		DeadSound = DeadSoundRef.Object;
	}

	bTurnInPlace = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(200.f, 200.0f);

	// Movement
	DefaultWalkSpeed = 400.0f;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	// Mesh & AnimInstance
	GetMesh()->SetRelativeLocationAndRotation(FVector(-10.0f, 0.0f, -199.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(MeshScale, MeshScale, MeshScale));

	// AI
	AttackRange = 1550.0f * MeshScale;
	AIControllerClass = ASSKrakenAIController::StaticClass();
}

void ASS_Kraken::AttackHitCheck()
{
	Super::AttackHitCheck();

	if (HasAuthority())
	{
		FVector CollisionBoxCenter = GetActorLocation() + GetActorForwardVector() * 1200.0f * MeshScale;
		FVector CollisionBoxExtent = FVector(580.0f * MeshScale, 400.0f * MeshScale, 340.0f * MeshScale);


		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		TArray<FOverlapResult> OverlapResults;
		FCollisionShape CollisionBox = FCollisionShape::MakeBox(CollisionBoxExtent);

		bool bHitDetected = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			CollisionBoxCenter,
			GetActorQuat(),
			CCHANNEL_SSACTION,
			CollisionBox,
			CollisionParams);


#if ENABLE_DRAW_DEBUG
		FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
		DrawDebugBox(GetWorld(), CollisionBoxCenter, CollisionBoxExtent, GetActorRotation().Quaternion(), DrawColor, false, 2.0f);
#endif
	
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* OverlapActor = OverlapResult.GetActor();
			ASSCharacterPlayer* CharacterPlayer = Cast<ASSCharacterPlayer>(OverlapActor);

			if (CharacterPlayer)
			{
				FDamageEvent DamageEvent;
				const float AttackDamage = 30.0f;
				CharacterPlayer->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
			}
		}
	}
}

void ASS_Kraken::Tick(float DeltaSeconds)
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

	if (bTurnRight)
	{
		NetMulticastRpcShowAnimationMontageWithSection(TurnInPlaceMontage, TEXT("TurnRight"), AnimationSpeedRate);
	}

	else
	{
		NetMulticastRpcShowAnimationMontage(TurnInPlaceMontage, AnimationSpeedRate);
	}
}