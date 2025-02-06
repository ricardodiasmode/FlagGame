// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FlagAbility.generated.h"

/**
 * EFlagAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EFlagAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};


/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API UFlagAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VoxlifeDefaults|Ability Activation")
	EFlagAbilityActivationPolicy ActivationPolicy;

public:
	EFlagAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

};
