// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameplayPlayerController.h"
#include "BigMoxiGames/AbilitySystem/FlagAbilitySystemComponent.h"
#include "BigMoxiGames/GameplayFramework/HUDs/FlagHUD.h"
#include "BigMoxiGames/GameplayFramework/PlayerStates/FlagPlayerState.h"
#include "Engine/LocalPlayer.h"

void AGameplayPlayerController::BeginPlay()
{
	Super::BeginPlay();

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

void AGameplayPlayerController::Client_OnTeamWin_Implementation(const EPlayerTeam& PlayerTeam, const float TimeToResetAfterMatchEnd)
{
	if (GetHUD<AFlagHUD>())
		GetHUD<AFlagHUD>()->StartCounterOnTeamWin(PlayerTeam, TimeToResetAfterMatchEnd);
}
