#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "Occupation/PlayerTeam.h"
#include "EOSGameInstance.generated.h"

//퀵 조인 완료시 콜백해주는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickJoinSessionComplete,bool,IsSucsess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginCompleted,bool,IsSucsess);

enum class ERequestType
{
	None,
	ShowRecord,
	InsertRecord
};

USTRUCT()
struct FMatchResultStruct
{
	GENERATED_BODY()

	int64 StartTime;
	float Duration;
	EPlayerTeam WinTeam;
	TArray<FPlayerStats> AntiPlayers;
	TArray<FPlayerStats> ProPlayers;
};

USTRUCT()
struct FPlayerStats
{
	GENERATED_BODY()
	
	FString PlayerID;

	FString PlayerName;

	uint16 Kill; 

	uint16 Death;

	uint16 OccupationCount;

	uint16 OccupationTickCount;
};



UCLASS()
class LAKAYA_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEOSGameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void Login();
	void OnLoginComplete(int32 LocalUserNum,const bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void CreateSession();
	void OnCreateSessionComplete(FName SessionName,const bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnDestroySessionComplete(FName SessionName,const bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void FindSession();
	void OnFindSessionComplete(bool bWasSuccessful);
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void QuickJoinSession();
	void OnFindSessionCompleteWithQuickJoin(bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void GetAllFriends();
	void OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	UFUNCTION(BlueprintCallable)
	void ShowInviteUI();

	UFUNCTION(BlueprintCallable)
	void ShowFriendsUI();

	UFUNCTION(BlueprintCallable)
	void CallServerTravel();

	UFUNCTION(BlueprintCallable)
	void StartSession();
	void OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void EndSession();
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void PrintSessionState();

	UFUNCTION(BlueprintCallable)
	void CleanUpSession();

	void OnDestroySessionCompleteAndReJoinSession(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	bool IsLoggedIn();

	void Connect();
	
	bool RequestShowRecord();
	
	bool IsSocketConnected();

	bool HasPendingData();

	TArray<FMatchResultStruct> RecvMatchResultRecord();

	bool SendMatchResultData(const FMatchResultStruct& NewRecordResult);

private:
	static bool IsServer();

public:
	UPROPERTY(BlueprintAssignable,VisibleAnywhere, BlueprintCallable, Category = "Event")
	FOnQuickJoinSessionComplete OnQuickJoinSessionComplete;

	UPROPERTY(BlueprintAssignable,VisibleAnywhere, BlueprintCallable, Category = "Event")
	FOnLoginCompleted OnLoginCompleted;
	
protected:
	IOnlineSubsystem* OnlineSubsystem;

	IOnlineSessionPtr OnlineSessionPtr;

	bool bIsLoggedIn;

	FOnlineSessionSettings SessionSettings;

	APlayerController* MyPlayerController;
	//FName CurrentServerName;

	FUniqueNetIdPtr ClientNetId;

private:
	FSocket* SocketClient;
	ISocketSubsystem* SocketSubsystem;
	uint32 RecvDataSize;
};
