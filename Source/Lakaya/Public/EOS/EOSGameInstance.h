// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"

/**
 *
 */

//퀵 조인 완료시 콜백해주는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickJoinSessionComplete,bool,IsSucsess);

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
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void CreateSession();
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

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
	void OnEndSessionComplete(FName SessionName, bool bWasSiccessfil);

	UFUNCTION(BlueprintCallable)
	void PrintSessionState();

	void CleanUpSession();

	void OnDestorySessionCompleteAndReJoinSession(FName SessionName, bool bWasSuccessful);

public:
	UPROPERTY(BlueprintAssignable,VisibleAnywhere, BlueprintCallable, Category = "Event")
	FOnQuickJoinSessionComplete OnQuickJoinSessionComplete;
protected:
	class IOnlineSubsystem* OnlineSubsystem;

	bool bIsLoggedIn;

	FOnlineSessionSettings SessionSettings;

	APlayerController* MyPlayerController;
	//FName CurrentServerName;
};
