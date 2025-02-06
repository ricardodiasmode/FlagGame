// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FlagAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BIGMOXIGAMES_API UFlagAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

private:
	void ProcessAbilitiesWhenInputHeld(TArray<FGameplayAbilitySpecHandle>& AbilitiesToActivate);

	void ProcessAbilitiesWhenInputPressed(TArray<FGameplayAbilitySpecHandle>& AbilitiesToActivate);

	void ClearAbilityInput();

protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
public:
	// Sets default values for this component's properties
	UFlagAbilitySystemComponent();
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilitiesWhenInputReleased();
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

};
