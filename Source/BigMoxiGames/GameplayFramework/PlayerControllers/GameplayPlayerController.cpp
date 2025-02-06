// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameplayPlayerController.h"
#include "BigMoxiGames/AbilitySystem/FlagAbilitySystemComponent.h"
#include "BigMoxiGames/GameplayFramework/HUDs/FlagHUD.h"
#include "BigMoxiGames/GameplayFramework/PlayerStates/FlagPlayerState.h"
#include "BigMoxiGames/Sessions/SessionSubsystem.h"
#include "Engine/LocalPlayer.h"

void AGameplayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void AGameplayPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (GetPlayerState<AFlagPlayerState>())
	{
		if (UFlagAbilitySystemComponent* ASC = Cast<UFlagAbilitySystemComponent>(GetPlayerState<AFlagPlayerState>()->GetAbilitySystemComponent()))
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AGameplayPlayerController::CloseMenu()
{
	GetHUD<AFlagHUD>()->CloseMenu();
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void AGameplayPlayerController::EscPressed()
{
	if (GetHUD<AFlagHUD>())
	{
		if (GetHUD<AFlagHUD>()->bIsMenuOpen)
		{
			CloseMenu();
		} else
		{
			GetHUD<AFlagHUD>()->OpenMenu();
			SetInputMode(FInputModeUIOnly());
			SetShowMouseCursor(true);
		}
	}
}

void AGameplayPlayerController::ReturnToMenu()
{
	if (GetGameInstance())
	{
		if (GetGameInstance()->GetSubsystem<USessionSubsystem>())
			GetGameInstance()->GetSubsystem<USessionSubsystem>()->DestroySession();
	}
	
	FText UnusedReason;
	ClientReturnToMainMenuWithTextReason(UnusedReason);
}

void AGameplayPlayerController::Client_OnTeamWin_Implementation(const EPlayerTeam& PlayerTeam, const float TimeToResetAfterMatchEnd)
{
	if (GetHUD<AFlagHUD>())
		GetHUD<AFlagHUD>()->StartCounterOnTeamWin(PlayerTeam, TimeToResetAfterMatchEnd);
}
