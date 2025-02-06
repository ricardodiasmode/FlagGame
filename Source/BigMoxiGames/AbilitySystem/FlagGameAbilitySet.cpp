// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlagGameAbilitySet.h"
#include "AbilitySystemComponent.h"
#include "Misc/GeneralFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlagGameAbilitySet)

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void UFlagGameAbilitySet::RemoveAbility(UAbilitySystemComponent* ASC,
	const FAbilitySet_GameplayAbility& AbilityToRemove)
{
	int i = 0;
	while (i < ASC->GetActivatableAbilities().Num())
	{
		const FGameplayAbilitySpec& AbilitySpec = ASC->GetActivatableAbilities()[i];
		if (AbilitySpec.Ability->GetClass() == AbilityToRemove.Ability)
		{
			ASC->ClearAbility(AbilitySpec.Handle);
			break;
		}
		i++;
	}
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UFlagGameAbilitySet::UFlagGameAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UFlagGameAbilitySet::GrantAbility(UAbilitySystemComponent* ASC,
	const FAbilitySet_GameplayAbility& AbilityToGrant,
	FAbilitySet_GrantedHandles* OutGrantedHandles,
	UObject* SourceObject)
{
	UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
	AbilitySpec.SourceObject = SourceObject;
	AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
	UKismetSystemLibrary::PrintString(ASC->GetWorld(), FString::Printf(TEXT("getting ability: %s"), *AbilityToGrant.InputTag.ToString()), true, true, FColor::Yellow, 5.f);
	
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

	if (OutGrantedHandles)
	{
		OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
	}

	return AbilitySpecHandle.IsValid();
}

void UFlagGameAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		GrantAbility(ASC, AbilityToGrant, OutGrantedHandles, SourceObject);
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

