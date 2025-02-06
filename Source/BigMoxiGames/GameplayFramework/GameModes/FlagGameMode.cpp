// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagGameMode.h"

#include "BigMoxiGames/Enums/PlayerTeam.h"
#include "BigMoxiGames/GameplayFramework/PlayerControllers/GameplayPlayerController.h"
#include "BigMoxiGames/GameplayFramework/PlayerStates/FlagPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/GeneralFunctionLibrary.h"

AFlagGameMode::AFlagGameMode()
{
	bDelayedStart = true;
}

void AFlagGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

EPlayerTeam AFlagGameMode::SetTeamForNewPlayer(AController* Controller)
{
	// Refresh team, if inside one
	RemoveFromAnyTeam(Controller);
	
	const EPlayerTeam ValidTeam = RedTeamPlayers.Num() < BlueTeamPlayers.Num() ? EPlayerTeam::Red : EPlayerTeam::Blue;
	
	if (AGameplayPlayerController* NewPlayer = CastChecked<AGameplayPlayerController>(Controller))
	{
		if (NewPlayer->GetPlayerState<AFlagPlayerState>())
		{
			NewPlayer->GetPlayerState<AFlagPlayerState>()->Server_SetPlayerTeam(ValidTeam);

			if (ValidTeam == EPlayerTeam::Blue)
				BlueTeamPlayers.Add(NewPlayer);
			else
				RedTeamPlayers.Add(NewPlayer);
		} else
		{
			GPrintError("Could not get player state to assign team.");
		}
	} else
	{
		GPrintError("Could not get player controller to assign team.");
	}

	return ValidTeam;
}

AActor* AFlagGameMode::GetPlayerStartByTeam(const EPlayerTeam& PlayerTeam)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(),
		UEnum::GetValueAsName(PlayerTeam),
		OutActors);

	if (OutActors.IsEmpty())
		return nullptr;
	
	return OutActors[FMath::RandRange(0, OutActors.Num()-1)];
}

void AFlagGameMode::RemoveFromAnyTeam(AController* NewPlayer)
{
	if (RedTeamPlayers.Contains(NewPlayer))
		RedTeamPlayers.Remove(NewPlayer);
	if (BlueTeamPlayers.Contains(NewPlayer))
		BlueTeamPlayers.Remove(NewPlayer);
}

void AFlagGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
		return;

	if (NewPlayer->GetPawn())
		NewPlayer->GetPawn()->Destroy();

	const EPlayerTeam NewPlayerTeam = SetTeamForNewPlayer(NewPlayer);

	AActor* StartSpot = GetPlayerStartByTeam(NewPlayerTeam);

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}	
	}

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void AFlagGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	TArray<AActor*> OutActors;
	// This is not slow because it uses hash buckets
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), OutActors);

	if (!HasMatchStarted() && OutActors.Num() > 1)
		StartMatch();
}

void AFlagGameMode::NotifyAllPlayersGameEnded(const EPlayerTeam& WinnerTeam)
{
	TArray<AActor*> OutActors;
	// This is not slow because it uses hash buckets
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameplayPlayerController::StaticClass(), OutActors);

	for (AActor* CurrentController : OutActors)
	{
		if (AGameplayPlayerController* Controller = Cast<AGameplayPlayerController>(CurrentController))
			Controller->Client_OnTeamWin(WinnerTeam, TimeToResetAfterMatchEnd);
	}
}

bool AFlagGameMode::IsRestartingGame()
{
	return GetWorldTimerManager().IsTimerActive(MatchRestartHandle);
}

void AFlagGameMode::OnTeamWinGame(const EPlayerTeam& WinnerTeam)
{
	if (IsRestartingGame())
		return;

	NotifyAllPlayersGameEnded(WinnerTeam);

	ScheduleRestart();
}

void AFlagGameMode::OnPlayerDie(class AController* Controller)
{
	RestartPlayer(Controller);
}

void AFlagGameMode::ScheduleRestart()
{
	if (IsRestartingGame())
		return;

	EndMatch();

	GetWorldTimerManager().SetTimer(MatchRestartHandle,
		this,
		&ThisClass::RestartGame,
		TimeToResetAfterMatchEnd);
}
