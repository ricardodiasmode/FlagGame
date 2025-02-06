// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlagGameOnDestroySessionComplete, bool, Successful);

/**
 * 
 */
UCLASS()
class BIGMOXIGAMES_API USessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
public:
	FFlagGameOnDestroySessionComplete OnDestroySessionCompleteEvent;

protected:
	void OnDestroySessionCompleted(FName SessionName, bool Successful);

public:
	USessionSubsystem();

	UFUNCTION(BlueprintCallable)
	void DestroySession();
};
