// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "BigMoxiGames/Enums/PlayerTeam.h"
#include "GameFramework/PlayerState.h"
#include "GameplayEffectTypes.h"
#include "FlagPlayerState.generated.h"

class UGameplayEffect;
struct FAbilitySet_GameplayAbility;
class UFlagAbilitySystemComponent;
class UFlagGameAbilitySet;
class UBaseAttributeSet;
/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API AFlagPlayerState : public APlayerState,
	public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Config")
	TObjectPtr<UFlagAbilitySystemComponent> AbilityComponent;
	
	UPROPERTY(Transient)
	TObjectPtr<UBaseAttributeSet> AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UFlagGameAbilitySet> AbilitySet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<UGameplayEffect> ResetEffect;

	UPROPERTY(Replicated)
	EPlayerTeam PlayerTeam;

protected:
	virtual void BeginPlay() override;

public:
	void OnHealthChange(const FOnAttributeChangeData& OnAttributeChangeData);
	
	AFlagPlayerState();
	
	///~ IAbilitySystemInterface start

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	///~ IAbilitySystemInterface end

	virtual UBaseAttributeSet* GetAttributeSet() const;

	void GrantAbility(const FAbilitySet_GameplayAbility& Ability);
	void RemoveAbility(const FAbilitySet_GameplayAbility& Ability);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerTeam(const EPlayerTeam& ValidTeam);

	bool IsRedTeam() const { return PlayerTeam == EPlayerTeam::Red; }

	void OnReset();
};
