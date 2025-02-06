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
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponEquipped(AWeapon* Weapon);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponDequipped(AWeapon* Weapon);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartCounterOnTeamWin(const EPlayerTeam& PlayerTeam, const float TimeToResetAfterMatchEnd);
};
