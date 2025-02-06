// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootAbility.h"

#include "BigMoxiGames/Pickables/Weapon.h"

void UShootAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	if (!HasAuthority(&ActivationInfo))
		return;

	if (const AWeapon* Weapon = Cast<AWeapon>(ActorInfo->AvatarActor.Get()))
		Weapon->Server_Fire();
	
	constexpr bool bReplicateEndAbility = true;
	constexpr bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
