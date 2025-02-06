// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlagAbility.h"
#include "ShootAbility.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API UShootAbility : public UFlagAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
