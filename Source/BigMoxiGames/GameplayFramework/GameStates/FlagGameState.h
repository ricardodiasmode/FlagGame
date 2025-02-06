// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Actors/PlayerBase.h"
#include "GameFramework/GameState.h"
#include "FlagGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScoreChangedSignature);

/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API AFlagGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing=ScoreChanged, BlueprintReadOnly)
	int RedTeamPoints = 0;
	
	UPROPERTY(ReplicatedUsing=ScoreChanged, BlueprintReadOnly)
	int BlueTeamPoints = 0;
	
	UPROPERTY(EditDefaultsOnly)
	int PointsToGameReset = 3;

	UPROPERTY(BlueprintAssignable)
	FScoreChangedSignature ScoreChangedDelegate;

private:
	UFUNCTION(Server, reliable)
	void Server_CheckMatchShouldOver();

	UFUNCTION()
	void ScoreChanged();
	
public:
	void OnTeamWin(const EPlayerTeam BaseTeam);
};
