// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UWeaponAttributeSet, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWeaponAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);
}

void UWeaponAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWeaponAttributeSet, Ammo, OldAmmo);
}

void UWeaponAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWeaponAttributeSet, MaxAmmo, OldMaxAmmo);
}

void UWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetAmmoAttribute())
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAmmo());
}

void UWeaponAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
		SetAmmo(FMath::Clamp(GetAmmo(), 0.f, GetMaxAmmo()));
}