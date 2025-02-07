// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCharacter.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "BigMoxiGames/AbilitySystem/FlagAbilitySystemComponent.h"
#include "BigMoxiGames/AnimInstances/FirstPersonAnimInstance.h"
#include "BigMoxiGames/GameplayFramework/HUDs/FlagHUD.h"
#include "BigMoxiGames/GameplayFramework/PlayerControllers/GameplayPlayerController.h"
#include "BigMoxiGames/GameplayFramework/PlayerStates/FlagPlayerState.h"
#include "BigMoxiGames/Pickables/Weapon.h"
#include "BigMoxiGames/Structs/InputMappingContextAndPriority.h"
#include "BigMoxiGames/Tags/InputTags.h"
#include "Engine/LocalPlayer.h"
#include "Input/ASInputComponent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UserSettings/EnhancedInputUserSettings.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABigMoxiGamesCharacter

AGameplayCharacter::AGameplayCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Player should not see his 3rd person body
	GetMesh()->SetOwnerNoSee(true);
}

//////////////////////////////////////////////////////////////////////////// Input

void AGameplayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	const APlayerController* PC = Cast<APlayerController>(GetController());
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	check(InputMapping.Get());
	Subsystem->AddMappingContext(InputMapping.Get(), 0);

	UASInputComponent* ASIC = Cast<UASInputComponent>(InputComponent);
	if (ensureMsgf(ASIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
	{
		// Add the key mappings that may have been set by the player
		ASIC->AddInputMappings(InputConfig, Subsystem);
		
		TArray<uint32> BindHandles;
		ASIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		
		ASIC->BindNativeAction(InputConfig, InputTags::InputTag_LMB, ETriggerEvent::Started, this, &ThisClass::Input_LMB, /*bLogIfNotFound=*/ false);

		ASIC->BindNativeAction(InputConfig, InputTags::InputTag_Jump, ETriggerEvent::Started, this, &ThisClass::Jump, /*bLogIfNotFound=*/ false);
		ASIC->BindNativeAction(InputConfig, InputTags::InputTag_Jump, ETriggerEvent::Completed, this, &ThisClass::StopJumping, /*bLogIfNotFound=*/ false);

		ASIC->BindNativeAction(InputConfig, InputTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Move, /*bLogIfNotFound=*/ false);
		ASIC->BindNativeAction(InputConfig, InputTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Look, /*bLogIfNotFound=*/ false);

		ASIC->BindNativeAction(InputConfig, InputTags::InputTag_ESC, ETriggerEvent::Completed, this, &ThisClass::Input_ESC, /*bLogIfNotFound=*/ false);
	}
}

void AGameplayCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AGameplayCharacter, AttachedWeapon, COND_None, REPNOTIFY_Always);
}

void AGameplayCharacter::SpawnAndEquipWeapon()
{
	if (StartWeaponClass)
	{
		AttachedWeapon = GetWorld()->SpawnActor<AWeapon>(StartWeaponClass);
		AttachedWeapon->AttachWeapon(this);
	}
}

void AGameplayCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AttachedWeapon)
		AttachedWeapon->Destroy();
}

void AGameplayCharacter::OnRep_AttachedWeapon()
{
	if (GetController<APlayerController>())
	{
		if (AFlagHUD* HUD = Cast<AFlagHUD>(GetController<APlayerController>()->GetHUD()))
		{
			if (AttachedWeapon)
				HUD->OnWeaponEquipped();
			else
				HUD->OnWeaponDequipped();
		}
	}
}

void AGameplayCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UFlagAbilitySystemComponent* ASC = Cast<UFlagAbilitySystemComponent>(GetPlayerState<AFlagPlayerState>()->GetAbilitySystemComponent()))
		ASC->AbilityInputTagPressed(InputTag);
}

void AGameplayCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UFlagAbilitySystemComponent* ASC = Cast<UFlagAbilitySystemComponent>(GetPlayerState<AFlagPlayerState>()->GetAbilitySystemComponent()))
		ASC->AbilityInputTagReleased(InputTag);
}

void AGameplayCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGameplayCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGameplayCharacter::Server_LMB_Implementation()
{
	OnLeftMouseButtonPressedDelegate.Broadcast();
}

void AGameplayCharacter::Input_LMB()
{
	// Server should not send twice, but should receive from client
	if (GetLocalRole() != ROLE_Authority)
		Server_LMB();
	
	OnLeftMouseButtonPressedDelegate.Broadcast();
}

void AGameplayCharacter::Input_ESC()
{
	if (!GetController<AGameplayPlayerController>())
	{
		GPrintError("Controller not found on input esc.");
		return;
	}
	GetController<AGameplayPlayerController>()->EscPressed();
}

void AGameplayCharacter::WeaponDetached(AWeapon* Weapon)
{
	if (!Weapon)
		return;
	
	Weapon->OnWeaponFireDelegate.RemoveAll(this);
	Weapon->OnWeaponDetachedDelegate.RemoveAll(this);

	OnLeftMouseButtonPressedDelegate.RemoveAll(Weapon);

	AttachedWeapon = nullptr;
	OnRep_AttachedWeapon();
}

void AGameplayCharacter::WeaponFire()
{
	if (!IsValid(FireAnimation))
	{
		GPrintDebug("Could not play fire animation: it is nullptr.");
		return;
	}
	
	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
	if (AnimInstance != nullptr)
		AnimInstance->Montage_Play(FireAnimation, 1.f);
}

void AGameplayCharacter::WeaponAttached(AWeapon* Weapon)
{
	Weapon->OnWeaponFireDelegate.AddUniqueDynamic(this, &ThisClass::WeaponFire);
	Weapon->OnWeaponDetachedDelegate.AddUniqueDynamic(this, &ThisClass::WeaponDetached);

	AttachedWeapon = Weapon;
	
	OnRep_AttachedWeapon();
}

void AGameplayCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystemComponent();

	if (AFlagPlayerState* GameplayPlayerState = CastChecked<AFlagPlayerState>(GetPlayerState()))
		GetMesh()->SetMaterial(0, GameplayPlayerState->IsRedTeam() ? RedTeamMaterial : BlueTeamMaterial);
	
	SpawnAndEquipWeapon();
}

void AGameplayCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystemComponent();

	if (AFlagPlayerState* GameplayPlayerState = CastChecked<AFlagPlayerState>(GetPlayerState()))
		GetMesh()->SetMaterial(0, GameplayPlayerState->IsRedTeam() ? RedTeamMaterial : BlueTeamMaterial);
}

void AGameplayCharacter::InitAbilitySystemComponent()
{
	if (AFlagPlayerState* GameplayPlayerState = CastChecked<AFlagPlayerState>(GetPlayerState()))
	{
		GameplayPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GameplayPlayerState, this);
		GameplayPlayerState->OnReset();
	}}

bool AGameplayCharacter::HasWeapon() const
{
	return IsValid(AttachedWeapon);
}

UAbilitySystemComponent* AGameplayCharacter::GetAbilitySystemComponent()
{
	if (AFlagPlayerState* GameplayPlayerState = Cast<AFlagPlayerState>(GetPlayerState()))
		return GameplayPlayerState->GetAbilitySystemComponent();
	return nullptr;
}
