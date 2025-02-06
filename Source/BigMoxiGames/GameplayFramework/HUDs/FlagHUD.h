// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BigMoxiGames/Actors/PlayerBase.h"
#include "GameFramework/HUD.h"
#include "FlagHUD.generated.h"

class AWeapon;
/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API AFlagHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	uint8 bIsMenuOpen : 1 = false;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponEquipped();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponDequipped();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartCounterOnTeamWin(const EPlayerTeam& PlayerTeam, const float TimeToResetAfterMatchEnd);

	UFUNCTION(BlueprintImplementableEvent)
	void OpenMenu();

	UFUNCTION(BlueprintImplementableEvent)
	void CloseMenu();
};
