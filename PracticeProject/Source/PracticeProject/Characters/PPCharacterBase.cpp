// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PPCharacterBase.h"

APPCharacterBase::APPCharacterBase()
{
	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}


