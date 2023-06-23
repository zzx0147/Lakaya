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

//서버에 보내는 리퀘스트 종류 Enum입니다.
enum class ERequestType
{
	None,
	ShowRecord,
	InsertRecord
};

//게임 한 판의 결과를 저장하는 구조체입니다
//시작 시간, 게임 길이, 이긴 팀, 안티 플레이어, 프로 플레이어의 통계를 저장합니다
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


//한 판의 게임에서 한명의 플레이어에 대한 통계 구조체입니다
//플레이어ID,닉네임 ,킬, 데스, 점령 횟수, 점령한 점령지가 점수를 올린 틱의 횟수
//
USTRUCT()
struct FPlayerStats
{
	GENERATED_BODY()
	FPlayerStats(): Kill(0), Death(0), OccupationCount(0), OccupationTickCount(0){};
	FPlayerStats(const FString& PlayerID, const FString& PlayerName, uint16 Kill, uint16 Death, uint16 OccupationCount,
		uint16 OccupationTickCount)
		: PlayerID(PlayerID),
		  PlayerName(PlayerName),
		  Kill(Kill),
		  Death(Death),
		  OccupationCount(OccupationCount),
		  OccupationTickCount(OccupationTickCount){}
	
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

	//서버와 소켓을 연결합니다
	void Connect();

	//최근 10개의 내 게임 전적 정보를 요청합니다.
	bool RequestShowRecord();

	//소켓이 연결된 상태인지를 반환합니다
	bool IsSocketConnected();

	//서버로부터 전송된 데이터가 있는지를 반환합니다
	bool HasPendingData();

	//서버로부터 최근 10개의 내 게임 전적 정보를 수신합니다, 수신한 Json을 FMatchResultStruct 배열로 반환합니다
	TArray<FMatchResultStruct> RecvMatchResultRecord();

	//게임 한판이 종료되었을 때 해당 게임의 정보를 서버로 송신합니다
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
	//클라이언트용 소켓입니다
	FSocket* SocketClient;
	ISocketSubsystem* SocketSubsystem;
	uint32 RecvDataSize;
};
