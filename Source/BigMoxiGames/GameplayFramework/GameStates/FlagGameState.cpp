// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagGameState.h"

#include "BigMoxiGames/Enums/PlayerTeam.h"
#include "BigMoxiGames/GameplayFramework/GameModes/FlagGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AFlagGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AFlagGameState, RedTeamPoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AFlagGameState, BlueTeamPoints, COND_None, REPNOTIFY_Always);
}

void AFlagGameState::ScoreChanged()
{
	ScoreChangedDelegate.Broadcast();
}

void AFlagGameState::OnTeamWin(const EPlayerTeam BaseTeam)
{
	check(HasAuthority()); // Must only be called by server
	
	if (BaseTeam == EPlayerTeam::Blue)
		BlueTeamPoints++;
	else
		RedTeamPoints++;

	ScoreChanged();
	Server_CheckMatchShouldOver();
}

void AFlagGameState::Server_CheckMatchShouldOver_Implementation()
{
	if (BlueTeamPoints >= PointsToGameReset || RedTeamPoints >= PointsToGameReset)
	{
		if (AFlagGameMode* GameMode = Cast<AFlagGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			GameMode->OnTeamWinGame(RedTeamPoints >= PointsToGameReset ? EPlayerTeam::Red : EPlayerTeam::Blue);
	}
}
