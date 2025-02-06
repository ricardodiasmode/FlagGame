// Copyright Epic Games, Inc. All Rights Reserved.


#include "Weapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "TP_PickUpComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "BigMoxiGames/BigMoxiGamesProjectile.h"
#include "BigMoxiGames/AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "BigMoxiGames/GameplayFramework/Characters/GameplayCharacter.h"
#include "BigMoxiGames/GameplayFramework/PlayerStates/FlagPlayerState.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Misc/GeneralFunctionLibrary.h"

// Sets default values for this component's properties
AWeapon::AWeapon()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	RootComponent = WeaponMesh;

	PickupComponent = CreateDefaultSubobject<UTP_PickUpComponent>("PickupComponent");
	PickupComponent->SetupAttachment(RootComponent);
	
	AbilityComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilityComponent");
	AbilityComponent->SetIsReplicated(true);
	AbilityComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UWeaponAttributeSet>("AttributeSet");

	bReplicates = true;
	bAlwaysRelevant = true;
}

UAbilitySystemComponent* AWeapon::GetAbilitySystemComponent() const
{
	return AbilityComponent;
}

UWeaponAttributeSet* AWeapon::GetAttributeSet() const
{
	return AttributeSet;
}

void AWeapon::SpawnProjectile() const
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAttachParentActor()))
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = Character->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Owner = Character->GetPlayerState();

			// Spawn the projectile at the muzzle
			World->SpawnActor<ABigMoxiGamesProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
}

void AWeapon::Server_Fire_Implementation() const
{
	if (!IsValid(ProjectileClass))
	{
		GPrintError("Could not fire: ProjectileClass is nullptr.");
		return;
	}

	SpawnProjectile();
	
	OnWeaponFireDelegate.Broadcast();
	
	Multicast_PlayFireSound();
}

void AWeapon::Multicast_PlayFireSound_Implementation() const
{
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	} else
	{
		GPrintDebug("Not playing fire sound because it is nullptr.");
	}
}

void AWeapon::OnCharacterFire()
{
	if (HasAuthority())
		GetAbilitySystemComponent()->TryActivateAbilityByClass(FireAbility);
}

void AWeapon::AttachWeapon(AGameplayCharacter* TargetCharacter)
{
	// Checking if has weapon attached
	TArray<AActor*> OutActors;
	TargetCharacter->GetAttachedActors(OutActors);
	if (OutActors.FindItemByClass<AWeapon>())
		return;
	
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(TargetCharacter->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	TargetCharacter->WeaponAttached(this);

	TargetCharacter->OnLeftMouseButtonPressedDelegate.AddUniqueDynamic(this, &AWeapon::OnCharacterFire);
}

void AWeapon::GiveDefaultEffect()
{
	if (!IsValid(DefaultEffect))
	{
		GPrintError("Could not give default effect to weapon: it is nullptr.");
		return;
	}
	
	FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(DefaultEffect, 1.f, AbilityComponent->MakeEffectContext());
	AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AWeapon::InitializeAbilitySystemComponent()
{
	if (!IsValid(AbilityComponent))
	{
		GPrintError("Could not InitializeAbilitySystemComponent on weapon: AbilityComponent is nullptr.");
		return;
	}
	
	AbilityComponent->InitAbilityActorInfo(this, this);
	AbilityComponent->GiveAbility(FireAbility);
	GiveDefaultEffect();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystemComponent();
	
	PickupComponent->OnPickUp.AddUniqueDynamic(this, &ThisClass::AttachWeapon);
}

void AWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnWeaponDetachedDelegate.Broadcast(this);
}
