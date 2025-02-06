// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Actors/PlayerBase.h"
#include "BigMoxiGames/GameplayFramework/GameModes/FlagGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameplayPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class BIGMOXIGAMES_API AGameplayPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

public:
	UFUNCTION(Client, reliable)
	void Client_OnTeamWin(const EPlayerTeam& PlayerTeam, const float TimeToResetAfterMatchEnd);
};
