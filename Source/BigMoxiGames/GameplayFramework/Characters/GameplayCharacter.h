// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Interfaces/ProjectileTargetInterface.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayCharacter.generated.h"

struct FGameplayTag;
class AWeapon;
struct FInputMappingContextAndPriority;
class UASInputConfig;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftMouseButtonPressedSignature);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGameplayCharacter : public ACharacter,
	public IProjectileTargetInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P = nullptr;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent = nullptr;

protected:
	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UASInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, Category="Config")
	TObjectPtr<UInputMappingContext> InputMapping;
	
	UPROPERTY(EditAnywhere, Category="Config")
	UMaterialInstance* RedTeamMaterial = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Config")
	UMaterialInstance* BlueTeamMaterial = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Config")
	TSubclassOf<AWeapon> StartWeaponClass;
	
public:
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	FOnLeftMouseButtonPressedSignature OnLeftMouseButtonPressedDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_AttachedWeapon, BlueprintReadOnly)
	AWeapon* AttachedWeapon = nullptr;

private:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	UFUNCTION()
	void OnRep_AttachedWeapon();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, reliable)
	void Server_LMB();

	void Input_LMB();

	void Input_ESC();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	
	void SpawnAndEquipWeapon();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	AGameplayCharacter();

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION()
	void WeaponDetached(class AWeapon* Weapon);

	UFUNCTION()
	void WeaponFire();
	
	void WeaponAttached(AWeapon* Weapon);
	void InitAbilitySystemComponent();

	UFUNCTION(BlueprintPure)
	bool HasWeapon() const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() override;
};

