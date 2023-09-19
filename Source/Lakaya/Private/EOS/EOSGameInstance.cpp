// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS/EOSGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystem.h"

static constexpr int MaxPlayer = 6;

UEOSGameInstance::UEOSGameInstance()
{
	bIsLoggedIn = false;
	RecvDataSize = 32;
}

void UEOSGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Init Start"));

	OnlineSubsystem = IOnlineSubsystem::Get();
	if (ensure(OnlineSubsystem))
	{
		OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();

		if (ensure(OnlineSessionPtr))
		{
			UE_LOG_ONLINE(Log, TEXT("[%s]Found session interface"), *OnlineSubsystem->GetSubsystemName().ToString());
		}
	}

	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (ensure(SocketSubsystem))
	{
		SocketClient = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("SocketClient"), false);

		if (ensure(SocketClient))
		{
			UE_LOG_ONLINE(Log, TEXT("SocketClient Created"));
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, *OnlineSubsystem->GetSubsystemName().ToString());
}

void UEOSGameInstance::Shutdown()
{
	Super::Shutdown();
	if (IsServer())
	{
		DestroySession();
	}

	if (IsSocketConnected())
	{
		SocketClient->Close();
	}
}

void UEOSGameInstance::Login()
{
	if (!ensure(OnlineSubsystem && OnlineSubsystem->GetSubsystemName() == STEAM_SUBSYSTEM)) return;

	if (const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface(); ensure(Identity))
	{
		const FOnlineAccountCredentials Credentials(TEXT("accountportal"), TEXT(""),TEXT(""));

		// Credentials.Id = FString();
		// Credentials.Token = FString();
		// Credentials.Type = FString("accountportal");

		//Credentials.Id = FString("127.0.0.1:8081");
		//Credentials.Token = FString("Pollux");
		//Credentials.Type = FString("developer");

		Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
		Identity->Login(0, Credentials);
		MyPlayerController = GetFirstLocalPlayerController(GetWorld());
	}
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId,
                                       const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Display, TEXT("LoggedIn: %s"), *UserId.ToDebugString());
	}
	else
	{
		UE_LOG(LogOnline, Error, TEXT("Login Failed: %s"), *Error);
	}

	// 로그인 상태를 캐싱하는 것은 좋긴 하지만, 로그인 상태가 이후에 변경될 수도 있으므로 로그인 상태는 필요할 때 체크하도록 하는 것이 어떨까 싶습니다
	bIsLoggedIn = bWasSuccessful;
	OnLoginCompleted.Broadcast(bIsLoggedIn);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("Login Compelete"));

	if (!ensure(OnlineSubsystem)) return;
	if (const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface(); ensure(Identity))
	{
		Identity->ClearOnLoginCompleteDelegates(LocalUserNum, this);
		if (bWasSuccessful)
		{
			ClientNetId = Identity->GetUniquePlayerId(LocalUserNum);

			// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, ClientNetId->ToDebugString());
		}
	}
}


void UEOSGameInstance::CreateSession()
{
	//TODO: 중첩 분기문을 줄여봅시다..
	if (!bIsLoggedIn)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Create Session: Not Logged In"));
		return;
	}
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	if (MyPlayerController)
	{
		SessionSettings.bIsDedicated = false;
		SessionSettings.bShouldAdvertise = true;

		SessionSettings.bIsLANMatch = false;
		SessionSettings.NumPublicConnections = MaxPlayer;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
		SessionSettings.bAllowInvites = true;

		SessionSettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"),
		                    EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(FName(TEXT("ISJOINABLE")), true, EOnlineDataAdvertisementType::ViaOnlineService);

		OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddUObject(
			this, &UEOSGameInstance::OnCreateSessionComplete);

		const FUniqueNetIdPtr UserId = MyPlayerController->GetLocalPlayer()->GetPreferredUniqueNetId().
		                                                   GetUniqueNetId();
		const FName SessionName(NAME_GameSession);
		OnlineSessionPtr->CreateSession(*UserId, SessionName, SessionSettings);
		//SessionPtr->CreateSession(0, FName(NAME_GameSession), SessionSettings); 데디일때는 이거 써야함
	}
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, const bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session Success: %d"), bWasSuccessful);

	if (OnlineSubsystem == nullptr) return;

	if (!OnlineSessionPtr) return;

	OnlineSessionPtr->ClearOnCreateSessionCompleteDelegates(this);
	UE_LOG(LogTemp, Warning, TEXT("Start Game Level Open"));
	UGameplayStatics::OpenLevel(this, FName("LV_Completed_20230429"), true, FString("?listen"));

	//TODO: 이걸로 대체해보아요..
	// UGameplayStatics::OpenLevelBySoftObjectPtr()

	/*FString ConnectionInfo = FString();
	OnlineSessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);
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

void UEOSGameInstance::DestroySession()
{
	//TODO: 중첩 분기문을 줄여봅시다..
	if (!bIsLoggedIn) return;
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	OnlineSessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
	OnlineSessionPtr->DestroySession(NAME_GameSession);
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, const bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Destroy Session is %d"), bWasSuccessful));
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	OnlineSessionPtr->ClearOnDestroySessionCompleteDelegates(this);
}

void UEOSGameInstance::FindSession()
{
	if (!bIsLoggedIn) return;
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->MaxSearchResults = 5000;

	SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"), EOnlineComparisonOp::Equals);
	SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	OnlineSessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionComplete);
	OnlineSessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
}

void UEOSGameInstance::OnFindSessionComplete(const bool bWasSuccessful)
{
	//TODO: 중첩 분기문을 줄여봅시다..
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);

	if (!bWasSuccessful) return;
	if (!OnlineSubsystem) return;
	UE_LOG(LogTemp, Warning, TEXT("Found %d Lobbies"), SearchSettings->SearchResults.Num());
	if (!OnlineSessionPtr) return;

	//SessionPtr->ClearOnFindSessionsCompleteDelegates(this);
	if (SearchSettings->SearchResults.Num())
	{
		OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
		OnlineSessionPtr->JoinSession(0, NAME_GameSession, SearchSettings->SearchResults[0]);
	}
}

void UEOSGameInstance::OnJoinSessionComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("JoinSessionComplete!"));

	//TODO: 중첩 분기문을 줄여봅시다..
	if (OnlineSubsystem == nullptr) return;
	if (!OnlineSessionPtr) return;


	OnlineSessionPtr->ClearOnJoinSessionCompleteDelegates(this);
	FString ConnectionInfo = FString();
	OnlineSessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);

	if (!ConnectionInfo.IsEmpty())
	{
		if (ConnectionInfo.Equals(TEXT("EOS::GameNetDriver:26")))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Session is Available But No Server, Destroy Empty Session"));

			OnlineSessionPtr->OnDestroySessionCompleteDelegates.AddUObject(
				this, &UEOSGameInstance::OnDestroySessionCompleteAndReJoinSession);
			OnlineSessionPtr->DestroySession(SessionName);
			return;
			//OnCreateSessionComplete(NAME_GameSession, true);//만약 세션은 들어왔는데 서버는 없다면 직접 서버를 만든다
		}

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("ClientTravel Start!"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("ClientTravel Start!"));
			UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!%s"), *ConnectionInfo);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("!!!%s"), *ConnectionInfo));
			PC->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
			UE_LOG(LogTemp, Warning, TEXT("ClientTravel END!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerController is Empty!"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PlayerController is Empty!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ConnectionInfo is Empty!"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("ConnectionInfo is Empty!"));
	}
}

void UEOSGameInstance::QuickJoinSession()
{
	if (!bIsLoggedIn) return;
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->MaxSearchResults = 5000;

	SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"), EOnlineComparisonOp::Equals);
	SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);


	OnlineSessionPtr->OnFindSessionsCompleteDelegates.AddUObject(
		this, &UEOSGameInstance::OnFindSessionCompleteWithQuickJoin);

	const FUniqueNetIdPtr UserId = MyPlayerController->GetLocalPlayer()->GetPreferredUniqueNetId().
	                                                   GetUniqueNetId();

	OnlineSessionPtr->FindSessions(*UserId, SearchSettings.ToSharedRef());
}

void UEOSGameInstance::OnFindSessionCompleteWithQuickJoin(const bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Success: %d"),bWasSuccessful));
	if (!bWasSuccessful || OnlineSubsystem == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Found %d Lobbies"), SearchSettings->SearchResults.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Found %d Lobbies"), SearchSettings->SearchResults.Num()));

	bool IsSuccess = false;
	if (OnlineSessionPtr)
	{
		OnlineSessionPtr->ClearOnFindSessionsCompleteDelegates(this);
		if (SearchSettings->SearchResults.Num())
		{
			for (const FOnlineSessionSearchResult& Results : SearchSettings->SearchResults)
			{
				bool bIsJoinable;
				Results.Session.SessionSettings.Get(FName(TEXT("ISJOINABLE")), bIsJoinable);
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Lobby is joinable? %d"), isJoinable));

				//Results.Session.NumOpenPublicConnections;
				//Results.Session.SessionSettings.NumPublicConnections;

				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
				//                                 FString::Printf(
				//	                                 TEXT("NumOpenPublicConnections %d :: NumPublicConnections %d"),
				//	                                 Results.Session.NumOpenPublicConnections,
				//	                                 Results.Session.SessionSettings.NumPublicConnections));

				if (bIsJoinable && Results.Session.NumOpenPublicConnections > 0 && Results.Session.
					NumOpenPublicConnections < Results.Session.SessionSettings.NumPublicConnections)
				{
					const FUniqueNetIdPtr UserId = MyPlayerController->GetLocalPlayer()->GetPreferredUniqueNetId().
					                                                   GetUniqueNetId();

					IsSuccess = OnlineSessionPtr->JoinSession(*UserId, NAME_GameSession, Results);
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("try Join session, is success? %d"),IsSuccess));
					if (IsSuccess)
					{
						OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddUObject(
							this, &UEOSGameInstance::OnJoinSessionComplete);
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

void UEOSGameInstance::GetAllFriends()
{
	if (!bIsLoggedIn) return;
	if (!OnlineSubsystem) return;

	if (const IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
	{
		FriendsPtr->ReadFriendsList(0, FString(""),
		                            FOnReadFriendsListComplete::CreateUObject(
			                            this, &UEOSGameInstance::OnGetAllFriendsComplete));
	}
}

void UEOSGameInstance::OnGetAllFriendsComplete(int32 LocalUserNum, const bool bWasSuccessful, const FString& ListName,
                                               const FString& ErrorStr)
{
	UE_LOG(LogTemp, Warning, TEXT("CompleteGetAllFriends %d"), bWasSuccessful);
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("CompleteGetAllFriends %s"), *ErrorStr);
		return;
	}

	if (!OnlineSubsystem) return;

	if (const IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
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

void UEOSGameInstance::ShowInviteUI()
{
	if (!bIsLoggedIn) return;

	if (!OnlineSubsystem) return;

	if (const IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
	{
		UIPtr->ShowInviteUI(0, NAME_GameSession);
	}
}

void UEOSGameInstance::ShowFriendsUI()
{
	if (!bIsLoggedIn) return;

	if (!OnlineSubsystem) return;

	if (const IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
	{
		UIPtr->ShowFriendsUI(0);
	}
}

void UEOSGameInstance::CallServerTravel()
{
	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/MainLevel?listen");
}

void UEOSGameInstance::StartSession()
{
	if (!IsServer()) return;
	if (!OnlineSubsystem) return;

	if (!OnlineSessionPtr) return;

	FOnlineSessionSettings newSessionSettings;
	newSessionSettings.bIsDedicated = false;
	newSessionSettings.bShouldAdvertise = true;

	newSessionSettings.bIsLANMatch = false;
	newSessionSettings.NumPublicConnections = MaxPlayer;
	newSessionSettings.bAllowJoinInProgress = false;
	newSessionSettings.bAllowJoinViaPresence = true;
	newSessionSettings.bUsesPresence = true;
	newSessionSettings.bUseLobbiesIfAvailable = true;
	newSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	newSessionSettings.bAllowInvites = false;
	newSessionSettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"),
	                       EOnlineDataAdvertisementType::ViaOnlineService);
	newSessionSettings.Set(FName(TEXT("ISJOINABLE")), false,
	                       EOnlineDataAdvertisementType::ViaOnlineService);

	OnlineSessionPtr->OnUpdateSessionCompleteDelegates.AddUObject(
		this, &UEOSGameInstance::OnUpdateSessionComplete);
	const FName SessionName(NAME_GameSession);
	OnlineSessionPtr->UpdateSession(SessionName, newSessionSettings, true);
}

void UEOSGameInstance::OnUpdateSessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (SessionName != NAME_GameSession || !bWasSuccessful) return;

	if (!IsServer()) return;

	if (!OnlineSubsystem) return;

	if (!OnlineSessionPtr) return;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Update Session Complete"));
	OnlineSessionPtr->OnUpdateSessionCompleteDelegates.Clear();
}

void UEOSGameInstance::EndSession()
{
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	OnlineSessionPtr->EndSession(NAME_GameSession);
	//CleanUpSession();
	OnlineSessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnEndSessionComplete);
}

void UEOSGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!IsServer()) return;
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	OnlineSessionPtr->OnEndSessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnDestroySessionCompleteDelegates.AddUObject(
		this, &UEOSGameInstance::OnDestroySessionComplete);
	// OnlineSessionPtr->DestroySession(NAME_GameSession);
}

void UEOSGameInstance::PrintSessionState()
{
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	//EOnlineSessionState::Type State = SessionPtr->GetSessionState(NAME_GameSession);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, EOnlineSessionState::ToString(State));
}

void UEOSGameInstance::CleanUpSession()
{
	if (OnlineSubsystem == nullptr) return;

	if (!OnlineSessionPtr) return;

	const auto Session = OnlineSessionPtr->GetNamedSession(NAME_GameSession);

	if (Session == nullptr) return;

	if (Session->bHosting)
	{
		DestroySession();
	}
	else
	{
		EndSession();
	}

	// if (const EOnlineSessionState::Type SessionState = OnlineSessionPtr->GetSessionState(NAME_GameSession); EOnlineSessionState::InProgress == SessionState)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Ending session because of return to front end"));
	// 	OnlineSessionPtr->EndSession(NAME_GameSession);
	// 	OnlineSessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnEndSessionComplete);
	// }
	// else if (EOnlineSessionState::Ending == SessionState)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Waiting for session to end on return to main menu"));
	// }
	// else if (EOnlineSessionState::Ended == SessionState || EOnlineSessionState::Pending == SessionState)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Destroying session on return to main menu"));
	// 	OnlineSessionPtr->DestroySession(NAME_GameSession);
	// }
	// else if (EOnlineSessionState::Starting == SessionState || EOnlineSessionState::Creating == SessionState)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Waiting for session to start, and then we will end it to return to main menu"));
	// }
}

void UEOSGameInstance::OnDestroySessionCompleteAndReJoinSession(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("EmptySessionDestroyed ReJoin Start")));
	if (OnlineSubsystem == nullptr) return;

	if (!OnlineSessionPtr) return;

	OnlineSessionPtr->ClearOnDestroySessionCompleteDelegates(this);
	QuickJoinSession();
}

bool UEOSGameInstance::IsLoggedIn()
{
	return bIsLoggedIn;
}

void UEOSGameInstance::Connect()
{
	//IP 설정
	FIPv4Address IPAddress;
	FIPv4Address::Parse(TEXT("150.230.43.3"), IPAddress);
	constexpr int32 Port = 55165; 

	const TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	Addr->SetIp(IPAddress.Value);
	Addr->SetPort(Port);

	//연결
	if (bool bConnected = SocketClient->Connect(*Addr))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Socket Connect Success"));
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Socket Connect Fail"));
	}
	//연결 이후부터는 논블로킹으로 작동
	SocketClient->SetNonBlocking(true);
}

bool UEOSGameInstance::RequestShowRecord()
{
	//Json 오브젝트에 RequestType과 PlayerID를 저장합니다
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("RequestType"), TEXT("ShowRecord"));
	JsonObject->SetStringField(TEXT("PlayerID"), ClientNetId->ToString());

	//Json 오브젝트를 직렬화
	FString JsonRequestString;
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonRequestString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
	{
		// UTF-8로 인코딩 된 Json 문자열
		const FTCHARToUTF8 Utf8JsonString(*JsonRequestString);

		// Json 문자열을 TArray<uint8>로 변환
		TArray<uint8> DataToSend;
		DataToSend.Append((uint8*)Utf8JsonString.Get(), Utf8JsonString.Length());

		// FSocket으로 데이터 전송
		if (SocketClient)
		{
			int32 BytesSent;
			if (SocketClient->Send(DataToSend.GetData(), DataToSend.Num(), BytesSent))
			{
				// 데이터 전송 성공
				return true;
			}
		}
	}
	//데이터 전송 실패
	return false;
}


bool UEOSGameInstance::IsSocketConnected()
{
	//소켓이 생성되지 않았으면 false
	if (SocketClient == nullptr) return false;
	
	return ESocketConnectionState::SCS_Connected == SocketClient->GetConnectionState();
}

bool UEOSGameInstance::HasPendingData()
{
	//소켓이 생성되지 않았으면 false
	if (SocketClient == nullptr) return false;
	uint32 DataSize;
	return SocketClient->HasPendingData(DataSize);
}

TArray<FMatchResultStruct> UEOSGameInstance::RecvMatchResultRecord()
{
	TArray<FMatchResultStruct> Results;

	int32 Size;
	uint8 Buffer[10000];

	uint32 PendingDataSize;
	//서버로부터 수신한 데이터가 있는지 확인 후 수신
	if (SocketClient->HasPendingData(PendingDataSize))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Start Recv Match Result Record"));
		//데이터 수신
		SocketClient->Recv(Buffer, 10000, Size, ESocketReceiveFlags::None);
		
		// const FString ReceivedString = FString(Size, (const TCHAR*)Buffer);
		FUTF8ToTCHAR Utf8Converted((const ANSICHAR*)Buffer, Size);
		// 변환된 TCHAR를 FString으로 변환
		FString ConvertedString(Utf8Converted);


		//수신한 Json스트링을 Json 오브젝트로 역직렬화
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ConvertedString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			//Json 오브젝트로부터 데이터 추출
			const auto MatchRecordArray = JsonObject->GetArrayField(TEXT("MatchRecords"));
			
			for (const auto& MatchRecord : MatchRecordArray)
			{
				FMatchResultStruct MatchResultStruct;
				TSharedPtr<FJsonObject> ElementAsObject = MatchRecord->AsObject();
				
				if (ElementAsObject->HasField(TEXT("StartTime")))
				{
					// GEngine->AddOnScreenDebugMessage(-1, 300, FColor::Red, FString::Printf(TEXT("asefasefasefef")));
					const double DoubleValue = ElementAsObject->GetNumberField(TEXT("StartTime"));
					int64 Int64Value = static_cast<int64>(DoubleValue);
					MatchResultStruct.StartTime = Int64Value;
				}

				if (ElementAsObject->HasField(TEXT("Duration")))
				{
					const double DoubleValue = ElementAsObject->GetNumberField(TEXT("Duration"));
					MatchResultStruct.Duration = DoubleValue;
				}
				
				if (ElementAsObject->HasField(TEXT("WinTeam")))
				{
					int32 WinTeamValue = ElementAsObject->GetIntegerField(TEXT("WinTeam"));
					MatchResultStruct.WinTeam = WinTeamValue == 1 ? ETeam::Anti : ETeam::Pro;
				}

				const auto ProPlayersJson = ElementAsObject->GetArrayField(TEXT("ProPlayers"));
				const auto AntiPlayersJson = ElementAsObject->GetArrayField(TEXT("AntiPlayers"));

				for(const auto& PlayerElement : ProPlayersJson)
				{
					const TSharedPtr<FJsonObject> PlayerJsonObject = PlayerElement->AsObject();
					FPlayerStats PlayerStats;

					if(PlayerJsonObject->HasField(TEXT("PlayerName"))) PlayerStats.PlayerName = PlayerJsonObject->GetStringField(TEXT("PlayerName"));
					if(PlayerJsonObject->HasField(TEXT("PlayerID"))) PlayerStats.PlayerID = PlayerJsonObject->GetStringField(TEXT("PlayerID"));
					if(PlayerJsonObject->HasField(TEXT("Kill"))) PlayerStats.Kill = PlayerJsonObject->GetIntegerField(TEXT("Kill"));
					if(PlayerJsonObject->HasField(TEXT("Death"))) PlayerStats.Death = PlayerJsonObject->GetIntegerField(TEXT("Death"));
					if(PlayerJsonObject->HasField(TEXT("OccupationCount"))) PlayerStats.OccupationCount = PlayerJsonObject->GetIntegerField(TEXT("OccupationCount"));
					if(PlayerJsonObject->HasField(TEXT("OccupationTickCount"))) PlayerStats.OccupationTickCount = PlayerJsonObject->GetIntegerField(TEXT("OccupationTickCount"));
					
					MatchResultStruct.ProPlayers.Emplace(PlayerStats);
				}

				for(const auto& PlayerElement : AntiPlayersJson)
				{
					const TSharedPtr<FJsonObject> PlayerJsonObject = PlayerElement->AsObject();
					FPlayerStats PlayerStats;

					if(PlayerJsonObject->HasField(TEXT("PlayerName"))) PlayerStats.PlayerName = PlayerJsonObject->GetStringField(TEXT("PlayerName"));
					if(PlayerJsonObject->HasField(TEXT("PlayerID"))) PlayerStats.PlayerID = PlayerJsonObject->GetStringField(TEXT("PlayerID"));
					if(PlayerJsonObject->HasField(TEXT("Kill"))) PlayerStats.Kill = PlayerJsonObject->GetIntegerField(TEXT("Kill"));
					if(PlayerJsonObject->HasField(TEXT("Death"))) PlayerStats.Death = PlayerJsonObject->GetIntegerField(TEXT("Death"));
					if(PlayerJsonObject->HasField(TEXT("OccupationCount"))) PlayerStats.OccupationCount = PlayerJsonObject->GetIntegerField(TEXT("OccupationCount"));
					if(PlayerJsonObject->HasField(TEXT("OccupationTickCount"))) PlayerStats.OccupationTickCount = PlayerJsonObject->GetIntegerField(TEXT("OccupationTickCount"));
					
					MatchResultStruct.AntiPlayers.Emplace(PlayerStats);
				}
				
				//추출한 데이터를 결과 구조체 배열에 저장
				Results.Emplace(MatchResultStruct);
			}
			
		}
		else
		{
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Fail Deserialize Json"));
		}
	}

	return Results;
}

bool UEOSGameInstance::SendMatchResultData(const FMatchResultStruct& NewRecordResult)
{
	if (SocketClient == nullptr) return false;

	//Json 오브젝트 생성
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> MatchResultObject = MakeShareable(new FJsonObject());

	//Json 오브젝트에 RequestType, 시작 시간, 게임 길이, 이긴 팀 정보를 작성
	JsonObject->SetStringField(TEXT("RequestType"),TEXT("InsertRecord"));
	MatchResultObject->SetNumberField(TEXT("StartTime"), NewRecordResult.StartTime);
	MatchResultObject->SetNumberField(TEXT("Duration"), NewRecordResult.Duration);
	MatchResultObject->SetNumberField(TEXT("WinTeam"), static_cast<int32>(NewRecordResult.WinTeam));

	//PlayerStats를 Json으보젝트로 작성하는 함수
	auto ConvertPlayerStatsToJson = [](FPlayerStats PlayerStat)
	{
		TSharedRef<FJsonObject> PlayerJsonObject = MakeShared<FJsonObject>();
		PlayerJsonObject->SetStringField(TEXT("PlayerID"), PlayerStat.PlayerID);
		// GEngine->AddOnScreenDebugMessage(-1, 300, FColor::Green, *PlayerStat.PlayerID);
		PlayerJsonObject->SetStringField(TEXT("PlayerName"), PlayerStat.PlayerName);
		PlayerJsonObject->SetNumberField(TEXT("Kill"), PlayerStat.Kill);
		PlayerJsonObject->SetNumberField(TEXT("Death"), PlayerStat.Death);
		PlayerJsonObject->SetNumberField(TEXT("OccupationCount"), PlayerStat.OccupationCount);
		PlayerJsonObject->SetNumberField(TEXT("OccupationTickCount"), PlayerStat.OccupationTickCount);

		return PlayerJsonObject;
	};

	//두 플레이어 팀의 정보를 JsonValue배열로 작성
	TArray<TSharedPtr<FJsonValue>> AntiPlayersJsonArray;
	TArray<TSharedPtr<FJsonValue>> ProPlayersJsonArray;
	for (auto& PlayerStat : NewRecordResult.AntiPlayers)
	{
		AntiPlayersJsonArray.Add(MakeShared<FJsonValueObject>(ConvertPlayerStatsToJson(PlayerStat)));
	}
	for (auto& PlayerStat : NewRecordResult.ProPlayers)
	{
		ProPlayersJsonArray.Add(MakeShared<FJsonValueObject>(ConvertPlayerStatsToJson(PlayerStat)));
	}

	//만든 배열을 JsonObject에 작성
	MatchResultObject->SetArrayField(TEXT("AntiPlayers"), AntiPlayersJsonArray);
	MatchResultObject->SetArrayField(TEXT("ProPlayers"), ProPlayersJsonArray);
	JsonObject->SetObjectField(TEXT("MatchResult"), MatchResultObject);

	//만든 Json 오브젝트를 직렬화
	FString JsonRequestString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonRequestString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
	{
		// UTF-8로 인코딩된 JSON 문자열 변환
		FTCHARToUTF8 Utf8JsonString(*JsonRequestString);

		// GEngine->AddOnScreenDebugMessage(-1, 300, FColor::Green, *JsonRequestString);

		// JSON 문자열을 TArray<uint8>로 변환
		TArray<uint8> DataToSend;
		DataToSend.Empty();
		// DataToSend.Append((uint8*)*JsonRequestString, JsonRequestString.Len());
		DataToSend.Append((uint8*)Utf8JsonString.Get(), Utf8JsonString.Length());

		// 전송
		int32 BytesSent;
		if (SocketClient->Send(DataToSend.GetData(), DataToSend.Num(), BytesSent))
		{
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("RecordDataSend Success"));
			// 데이터 전송 성공
			return true;
		}
		else
		{
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("RecordDataSend Fail"));
			// 데이터 전송 실패
			return false;
		}
	}

	return false;
}


void UEOSGameInstance::CreateDedicatedSession()
{
	if (!OnlineSubsystem) return;
	if (!OnlineSessionPtr) return;

	SessionSettings.bIsDedicated = true;
	SessionSettings.bShouldAdvertise = true;

	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = MaxPlayer;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings.bAllowInvites = true;

	SessionSettings.Set(SEARCH_KEYWORDS, FString("LakayaLobby"),
	                    EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName(TEXT("ISJOINABLE")), true, EOnlineDataAdvertisementType::ViaOnlineService);

	OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddUObject(
		this, &UEOSGameInstance::OnCreateSessionComplete);
	
	const FName SessionName(NAME_GameSession);
	OnlineSessionPtr->CreateSession(0, FName(NAME_GameSession), SessionSettings);//데디일때
}

bool UEOSGameInstance::IsServer()
{
	return UKismetSystemLibrary::IsServer(GEngine->GetWorld()) || UKismetSystemLibrary::IsDedicatedServer(
		GEngine->GetWorld());
}
