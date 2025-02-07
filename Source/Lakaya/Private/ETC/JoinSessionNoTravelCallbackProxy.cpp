// Copyright Epic Games, Inc. All Rights Reserved.

#include "ETC/JoinSessionNoTravelCallbackProxy.h"

#include "FindSessionsCallbackProxy.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "AdvancedSessions/Classes/BlueprintDataDefinitions.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(JoinSessionNoTravelCallbackProxy)

//////////////////////////////////////////////////////////////////////////
// UJoinSessionCallbackProxy

UJoinSessionNoTravelCallbackProxy::UJoinSessionNoTravelCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UJoinSessionNoTravelCallbackProxy* UJoinSessionNoTravelCallbackProxy::JoinSessionNoTravel(UObject* WorldContextObject, class APlayerController* PlayerController, const FBlueprintSessionResult& SearchResult)
{
	UJoinSessionNoTravelCallbackProxy* Proxy = NewObject<UJoinSessionNoTravelCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->OnlineSearchResult = SearchResult.OnlineResult;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UJoinSessionNoTravelCallbackProxy::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("JoinSession"), WorldContextObject->GetWorld());
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			DelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(Delegate);
			Sessions->JoinSession(*Helper.UserID, NAME_GameSession, OnlineSearchResult);

			// OnCompleted will get called, nothing more to do now
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

void UJoinSessionNoTravelCallbackProxy::OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("JoinSessionCallback"), WorldContextObject->GetWorld());
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());
	
	
	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(DelegateHandle);

			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				// Client travel to the server
				FString ConnectString;
				if (Sessions->GetResolvedConnectString(NAME_GameSession, ConnectString) && PlayerControllerWeakPtr.IsValid())
				{
					UE_LOG_ONLINE_SESSION(Log, TEXT("Join session: traveling to %s"), *ConnectString);
					// PlayerControllerWeakPtr->ClientTravel(ConnectString, TRAVEL_Absolute);
					OnSuccess.Broadcast(ConnectString);
					return;
				}
			}
		}
	}

	OnFailure.Broadcast();
}