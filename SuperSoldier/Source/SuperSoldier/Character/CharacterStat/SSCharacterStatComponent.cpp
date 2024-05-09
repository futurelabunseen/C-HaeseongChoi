// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStat/SSCharacterStatComponent.h"

USSCharacterStatComponent::USSCharacterStatComponent()
{
	MaxHP = 200.0f;
	CurrentHP = MaxHP;
}

void USSCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHP(MaxHP);
}

void USSCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, MaxHP);
	OnHpChanged.Broadcast(CurrentHP);

	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	UE_LOG(LogTemp, Log, TEXT("Current HP : %f"), CurrentHP)
}

float USSCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f, InDamage);

	SetHP(PrevHP - ActualDamage);

	return ActualDamage;
}
