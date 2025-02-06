// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BigMoxiGames/AbilitySystem/FlagGameAbilitySet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon.generated.h"

class UFlagGameAbilitySet;
class UWeaponAttributeSet;
class UTP_PickUpComponent;
class AGameplayCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponDetachedSignature, class AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireSignature);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIGMOXIGAMES_API AWeapon : public AActor,
	public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ABigMoxiGamesProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Gameplay, meta=(AllowPrivateAccess = "true"))
	UTP_PickUpComponent* PickupComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Config")
	TObjectPtr<UAbilitySystemComponent> AbilityComponent;
	
	UPROPERTY(Transient)
	TObjectPtr<UWeaponAttributeSet> AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<UGameplayAbility> FireAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<UGameplayEffect> DefaultEffect;

	FOnWeaponDetachedSignature OnWeaponDetachedDelegate;
	FOnWeaponFireSignature OnWeaponFireDelegate;

private:
	void GiveDefaultEffect();
	
	void InitializeAbilitySystemComponent();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** Sets default values for this component's properties */
	AWeapon();
	
	///~ IAbilitySystemInterface start

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	///~ IAbilitySystemInterface end

	virtual UWeaponAttributeSet* GetAttributeSet() const;
	
	void SpawnProjectile() const;

	UFUNCTION(Server, reliable)
	void Server_Fire() const;

	UFUNCTION(NetMulticast, reliable)
	void Multicast_PlayFireSound() const;

	UFUNCTION()
	void OnCharacterFire();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AGameplayCharacter* TargetCharacter);
};
