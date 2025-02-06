// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "CreateSessionCallbackProxyAdvanced.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineSessionInterface.h"


//////////////////////////////////////////////////////////////////////////
// UCreateSessionCallbackProxyAdvanced

UCreateSessionCallbackProxyAdvanced::UCreateSessionCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CreateCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateCompleted))
	, StartCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartCompleted))
{
}

UCreateSessionCallbackProxyAdvanced* UCreateSessionCallbackProxyAdvanced::CreateFlagGameSession(UObject* WorldContextObject,
	const TArray<FSessionPropertyKeyPair>& ExtraSettings,
	class APlayerController* PlayerController)
{
	UCreateSessionCallbackProxyAdvanced* Proxy = NewObject<UCreateSessionCallbackProxyAdvanced>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->ExtraSettings = ExtraSettings;
	return Proxy;
}

void UCreateSessionCallbackProxyAdvanced::Activate()
{
	if (PlayerControllerWeakPtr.Get() && PlayerControllerWeakPtr.Get()->PlayerState)
	{
		auto Sessions = Online::GetSessionInterface(WorldContextObject->GetWorld());
		if (Sessions.IsValid())
		{
			CreateCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegate);
			
			FOnlineSessionSettings Settings;
			Settings.NumPublicConnections = 99;
			Settings.NumPrivateConnections = 99;
			Settings.bShouldAdvertise = true;
			Settings.bAllowJoinInProgress = true;
			Settings.bIsLANMatch = true;
			Settings.bAllowJoinViaPresence = true;
			Settings.bIsDedicated = false;
			Settings.bUsesPresence = true;
			Settings.bUseLobbiesIfAvailable = true;

			Settings.bAntiCheatProtected = false;
			Settings.bUsesStats = false;
			Settings.bAllowInvites = true;
			
			FOnlineSessionSetting ExtraSetting;
			for (int i = 0; i < ExtraSettings.Num(); i++)
			{
				ExtraSetting.Data = ExtraSettings[i].Data;
				//	ViaOnlineServiceAndPing
				ExtraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
				Settings.Settings.Add(ExtraSettings[i].Key, ExtraSetting);
			}
			
			if (PlayerControllerWeakPtr.IsValid() && PlayerControllerWeakPtr.Get()->PlayerState->GetUniqueId().IsValid())
			{
				Sessions->CreateSession(*PlayerControllerWeakPtr.Get()->PlayerState->GetUniqueId(), NAME_GameSession, Settings);
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("Invalid Player controller when attempting to start a session"), ELogVerbosity::Warning);
				Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
					
				// Fail immediately
				OnFailure.Broadcast();
			}

			// OnCreateCompleted will get called, nothing more to do now
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
		}
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void UCreateSessionCallbackProxyAdvanced::OnCreateCompleted(FName SessionName, bool bWasSuccessful)
{
	if (PlayerControllerWeakPtr.Get() && PlayerControllerWeakPtr.Get()->PlayerState)
	{
		auto Sessions = Online::GetSessionInterface(WorldContextObject->GetWorld());
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
			
			if (bWasSuccessful)
			{
				UE_LOG_ONLINE_SESSION(Display, TEXT("Session creation completed. Automatic start is turned on, starting session now."));
				StartCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(StartCompleteDelegate);
				Sessions->StartSession(NAME_GameSession); // We'll call `OnSuccess.Broadcast()` when start succeeds.

				// OnStartCompleted will get called, nothing more to do now
				return;
			}
		}
	}

	if (!bWasSuccessful)
	{
		OnFailure.Broadcast();
	}
}

void UCreateSessionCallbackProxyAdvanced::OnStartCompleted(FName SessionName, bool bWasSuccessful)
{
	if (PlayerControllerWeakPtr.Get() && PlayerControllerWeakPtr.Get()->PlayerState)
	{
		auto Sessions = Online::GetSessionInterface(WorldContextObject->GetWorld());
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
}
