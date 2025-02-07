// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Enums/PlayerTeam.h"
#include "GameFramework/GameMode.h"
#include "FlagGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API AFlagGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	FTimerHandle MatchRestartHandle;
	
	UPROPERTY()
	TArray<AController*> RedTeamPlayers;
	
	UPROPERTY()
	TArray<AController*> BlueTeamPlayers;

	UPROPERTY(EditDefaultsOnly)
	float TimeToResetAfterMatchEnd = 10.f;

private:
	EPlayerTeam SetTeamForNewPlayer(AController* Controller);

	void ScheduleRestart();

	void NotifyAllPlayersGameEnded(const EPlayerTeam& WinnerTeam);
	
	bool IsRestartingGame();

protected:
	AFlagGameMode();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	AActor* GetPlayerStartByTeam(const EPlayerTeam& PlayerTeam);
	
	void RemoveFromAnyTeam(AController* NewPlayer);

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void OnTeamWinGame(const EPlayerTeam& WinnerTeam);
	
	void OnPlayerDie(class AController* Controller);
};
