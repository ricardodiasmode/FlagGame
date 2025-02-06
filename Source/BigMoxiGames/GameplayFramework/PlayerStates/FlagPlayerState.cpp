// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagPlayerState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BigMoxiGames/AbilitySystem/FlagAbilitySystemComponent.h"
#include "BigMoxiGames/AbilitySystem/FlagGameAbilitySet.h"
#include "BigMoxiGames/AbilitySystem/AttributeSets/BaseAttributeSet.h"
#include "BigMoxiGames/GameplayFramework/GameModes/FlagGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

AFlagPlayerState::AFlagPlayerState()
{
	AbilityComponent = CreateDefaultSubobject<UFlagAbilitySystemComponent>("AbilityComponent");
	AbilityComponent->SetIsReplicated(true);
	AbilityComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>("AttributeSet");
	AbilityComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChange);
}

void AFlagPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AFlagPlayerState, PlayerTeam, COND_None, REPNOTIFY_Always);
}

void AFlagPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AbilitySet->GiveToAbilitySystem(AbilityComponent, nullptr);
}

void AFlagPlayerState::OnHealthChange(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!HasAuthority())
		return;
	
	if (OnAttributeChangeData.NewValue <= 0)
	{
		if (AFlagGameMode* GameMode = Cast<AFlagGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			GameMode->OnPlayerDie(GetOwningController());
	}
}

UAbilitySystemComponent* AFlagPlayerState::GetAbilitySystemComponent() const
{
	return AbilityComponent;
}

UBaseAttributeSet* AFlagPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AFlagPlayerState::GrantAbility(const FAbilitySet_GameplayAbility& Ability)
{
	if (AbilitySet)
		AbilitySet->GrantAbility(AbilityComponent, Ability, nullptr);
}

void AFlagPlayerState::RemoveAbility(const FAbilitySet_GameplayAbility& Ability)
{
	if (AbilitySet)
		AbilitySet->RemoveAbility(AbilityComponent, Ability);
}

void AFlagPlayerState::OnReset()
{
	if (ResetEffect)
	{
		const UGameplayEffect* GameplayEffect = ResetEffect->GetDefaultObject<UGameplayEffect>();
		AbilityComponent->ApplyGameplayEffectToSelf(GameplayEffect, 1, AbilityComponent->MakeEffectContext());
	} else
	{
		GPrintError("ResetEffect is nullptr.");
	}
}

void AFlagPlayerState::Server_SetPlayerTeam_Implementation(const EPlayerTeam& ValidTeam)
{
	PlayerTeam = ValidTeam;
}
