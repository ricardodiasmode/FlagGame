// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BigMoxiGames/Structs/SessionPropertyKeyPair.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "CreateSessionCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UCreateSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	/**
	 *    Creates a session with the default online subsystem with advanced optional inputs.
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject",AutoCreateRefTerm="ExtraSettings"), Category = "Online|AdvancedSessions")
	static UCreateSessionCallbackProxyAdvanced* CreateFlagGameSession(UObject* WorldContextObject,
		const TArray<FSessionPropertyKeyPair>& ExtraSettings,
		class APlayerController* PlayerController);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when session creation completes, optionally calls StartSession
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);

	// Internal callback when session start completes
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);

	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The delegate executed by the online subsystem
	FOnCreateSessionCompleteDelegate CreateCompleteDelegate;

	// The delegate executed by the online subsystem
	FOnStartSessionCompleteDelegate StartCompleteDelegate;

	// Handles to the registered delegates above
	FDelegateHandle CreateCompleteDelegateHandle;
	FDelegateHandle StartCompleteDelegateHandle;

	// Store extra settings
	TArray<FSessionPropertyKeyPair> ExtraSettings;

	// The world context object in which this call is taking place
	TWeakObjectPtr<UObject> WorldContextObject;
};

