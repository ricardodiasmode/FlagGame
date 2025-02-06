// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "WeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API UWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_Ammo)
	FGameplayAttributeData Ammo;
	UFUNCTION()
	void OnRep_Ammo(const FGameplayAttributeData& OldAmmo) const;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, Ammo);
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	UFUNCTION()
	void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo) const;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, MaxAmmo);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
