// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "OnlineSubSystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/DateTime.h"

//const FName MyGameSessionName = FName(TEXT("Game Session"));
const FName MyGameSessionName = FName("Game Session");

UEOSGameInstance::UEOSGameInstance()
{
	bIsLoggedIn = false;
}

void UEOSGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Init Start"));

	OnlineSubsystem = IOnlineSubsystem::Get();

	//Login();

}

void UEOSGameInstance::Shutdown()
{
	Super::Shutdown();
	if (UKismetSystemLibrary::IsServer(GEngine->GetWorld()))
	{
		DestroySession();
	}
}

void UEOSGameInstance::Login()
{
	UE_LOG(LogTemp, Warning, TEXT("Login start"));
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;

			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");

			//Credentials.Id = FString("127.0.0.1:8081");
			//Credentials.Token = FString("Pollux");
			//Credentials.Type = FString("developer");

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
			Identity->Login(0, Credentials);
		}
	}
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("LoggedIn: %d"), bWasSuccessful);
	bIsLoggedIn = bWasSuccessful;

	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}


void UEOSGameInstance::CreateSession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				//FDateTime Now = FDateTime::UtcNow();
				//int64 Timestamp = Now.ToUnixTimestamp();
				//FString SessionName = FString::Printf("Lakaya:%lld", Timestamp);
				


				SessionSettings.bIsDedicated = false;
				SessionSettings.bShouldAdvertise = true;

				SessionSettings.bIsLANMatch = false;
				SessionSettings.NumPublicConnections = 5;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bUsesPresence = true;
				SessionSettings.bUseLobbiesIfAvailable = true;

				SessionSettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
				SessionPtr->CreateSession(0, MyGameSessionName, SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Create Session: Not Logged In"));
	}
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session Success: %d"), bWasSuccessful);

	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
			UE_LOG(LogTemp,Warning,TEXT("Start Game Level Open"));
			UGameplayStatics::OpenLevel(this, FName("TestGamePlayLevel"), true, FString("?listen"));

			/*FString ConnectionInfo = FString();
			SessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);
			if (!ConnectionInfo.IsEmpty())
			{
				if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					UE_LOG(LogTemp, Warning, TEXT("Server::ClientTravel Start!"));
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server::ClientTravel Start!"));
					UE_LOG(LogTemp, Warning, TEXT("Server::ConnectionInfo %s"), *ConnectionInfo);
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Server::ConnectionInfo %s"), *ConnectionInfo));
					PC->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Server::PlayerController is Empty!"));
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server::PlayerController is Empty!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Server::ConnectionInfo is Empty!"));
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server::ConnectionInfo is Empty!"));
			}*/
		}
	}
}

void UEOSGameInstance::DestroySession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
				SessionPtr->DestroySession(MyGameSessionName);
			}
		}
	}
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
		}
	}

}

void UEOSGameInstance::FindSession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SearchSettings = MakeShareable(new FOnlineSessionSearch());
				SearchSettings->MaxSearchResults = 5000;

				SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"), EOnlineComparisonOp::Equals);
				SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
				
				SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionComplete);
				SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
			}
		}
	}
}

void UEOSGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %d Lobbies"),SearchSettings->SearchResults.Num());
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				//SessionPtr->ClearOnFindSessionsCompleteDelegates(this);
				if(SearchSettings->SearchResults.Num())
				{ 
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
					//SearchSettings->SearchResults[0].Session.SessionSettings.Get();
					//SearchSettings->SearchResults[0].GetSessionIdStr();
					SessionPtr->JoinSession(0,MyGameSessionName,SearchSettings->SearchResults[0]);
				}
			}
		}
	}
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("JoinSessionComplete!"));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("JoinSessionComplete"));

	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			FString ConnectionInfo = FString();
			SessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);

			if (!ConnectionInfo.IsEmpty())
			{
				if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					UE_LOG(LogTemp, Warning, TEXT("ClientTravel Start!"));
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("ClientTravel Start!"));
					UE_LOG(LogTemp, Warning, TEXT("ConnectionInfo %s"), *ConnectionInfo);
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ConnectionInfo %s"), *ConnectionInfo));
					PC->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
					UE_LOG(LogTemp, Warning, TEXT("ClientTravel END!"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PlayerController is Empty!"));
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PlayerController is Empty!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionInfo is Empty!"));
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("ConnectionInfo is Empty!"));
			}
		}
	}

}

void UEOSGameInstance::QuickJoinSession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SearchSettings = MakeShareable(new FOnlineSessionSearch());
				SearchSettings->MaxSearchResults = 5000;

				SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"), EOnlineComparisonOp::Equals);
				SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

				SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionCompleteWithQuickJoin);
				SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
			}
		}
	}
}

void UEOSGameInstance::OnFindSessionCompleteWithQuickJoin(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Success: %d"),bWasSuccessful));
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %d Lobbies"), SearchSettings->SearchResults.Num());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Found %d Lobbies"), SearchSettings->SearchResults.Num()));
		if (OnlineSubsystem)
		{
			bool IsSuccess = false;
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				//SessionPtr->ClearOnFindSessionsCompleteDelegates(this);
				if (SearchSettings->SearchResults.Num())
				{
					for (const auto Results : SearchSettings->SearchResults)
					{
						if (Results.Session.SessionSettings.bAllowJoinInProgress)
						{
							IsSuccess = SessionPtr->JoinSession(0, MyGameSessionName, Results);
							if (IsSuccess)
							{
								SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
								break;
							}

						}
					}
				}

			}
			if (OnQuickJoinSessionComplete.IsBound())
			{
				OnQuickJoinSessionComplete.Broadcast(IsSuccess);
				OnQuickJoinSessionComplete.Clear();
			}
		}
	}
}

void UEOSGameInstance::GetAllFriends()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				FriendsPtr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UEOSGameInstance::OnGetAllFriendsComplete));
			}
		}
	}

}

void UEOSGameInstance::OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	UE_LOG(LogTemp, Warning, TEXT("CompleteGetAllFriends %d"),bWasSuccessful);
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("CompleteGetAllFriends %s"), *ErrorStr);
	}

	//if (bWasSuccessful)
	//{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				TArray<TSharedRef<FOnlineFriend>> FriendsList;
				if (FriendsPtr->GetFriendsList(0, ListName, FriendsList))
				{
					for (TSharedRef<FOnlineFriend> Friend : FriendsList)
					{
						FString FriendName = Friend.Get().GetRealName();
						UE_LOG(LogTemp, Warning, TEXT("Friend: %s"), *FriendName);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed GetFriendsList"));
				}
			}
		}
	//}
}

void UEOSGameInstance::ShowInviteUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
			{
				UIPtr->ShowInviteUI(0,MyGameSessionName);
			}
		}
	}
}

void UEOSGameInstance::ShowFriendsUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
			{
				UIPtr->ShowFriendsUI(0);
			}
		}
	}
}

void UEOSGameInstance::CallServerTravel()
{
	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/MainLevel?listen");
}

void UEOSGameInstance::StartSession()
{
	if (UKismetSystemLibrary::IsServer(GetWorld()) || UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionSettings.bAllowJoinInProgress = false;
				SessionPtr->OnUpdateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnUpdateSessionComplete);
				SessionPtr->UpdateSession(MyGameSessionName, SessionSettings);
			}
		}
	}
}

void UEOSGameInstance::OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName == MyGameSessionName && bWasSuccessful)
	{
		if (UKismetSystemLibrary::IsServer(GetWorld()) || UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
		{
			if (OnlineSubsystem)
			{
				if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Update Session Complete"));
					SessionPtr->OnUpdateSessionCompleteDelegates.Clear();
					SessionPtr->StartSession(MyGameSessionName);

				}
			}
		}
	}

}

void UEOSGameInstance::EndSession()
{
	if (UKismetSystemLibrary::IsServer(GetWorld()) || UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->EndSession(MyGameSessionName);
				SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnEndSessionComplete);
			}
		}
	}
}

void UEOSGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSiccessfil)
{
	if (UKismetSystemLibrary::IsServer(GetWorld()) || UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnEndSessionCompleteDelegates.Clear();
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
				SessionPtr->DestroySession(MyGameSessionName);
			} 
		}
	}
}

void UEOSGameInstance::PrintSessionState()
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			EOnlineSessionState::Type State = SessionPtr->GetSessionState(MyGameSessionName);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, EOnlineSessionState::ToString(State));
		}
	}
}
