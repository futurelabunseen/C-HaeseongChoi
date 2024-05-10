// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterStat/SSCharacterStatComponent.h"
#include "Net/UnrealNetwork.h"

USSCharacterStatComponent::USSCharacterStatComponent()
{
	MaxHP = 200.0f;
	CurrentHP = MaxHP;
}

void USSCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
	SetHP(MaxHP);
}

void USSCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USSCharacterStatComponent, CurrentHP);
}

void USSCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, MaxHP);
	OnHpChanged.Broadcast(CurrentHP);

	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}

void USSCharacterStatComponent::OnRep_CurrentHP()
{
	OnHpChanged.Broadcast(CurrentHP);

	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}

float USSCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f, InDamage);

	SetHP(PrevHP - ActualDamage);

	return ActualDamage;
}
