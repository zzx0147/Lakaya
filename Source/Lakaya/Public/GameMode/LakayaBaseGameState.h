#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/GameState.h"
#include "LakayaBaseGameState.generated.h"

class UGameStateSequentialWidget;
DECLARE_EVENT_OneParam(ALakayaBaseGameState, OnChangePlayerNumberSignature, const uint8&)

DECLARE_EVENT_ThreeParams(ALakayaBaseGameState, FOnPlayerKillNotifiedSignature, APlayerState*, APlayerState*, AActor*)

UCLASS()
class LAKAYA_API ALakayaBaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	ALakayaBaseGameState();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void HandleMatchIsCharacterSelect();
	virtual void HandleMatchIsIntro();
	virtual void OnRep_MatchState() override;
	
public:
	// 플레이가능한 최대 인원 수를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const uint8& GetMaximumPlayers() const { return MaximumPlayers; }

	// 현재의 인원 수를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	uint8 GetPlayersNumber() const { return PlayerArray.Num(); }

	// 매치가 종료되는 시간을 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const float& GetMatchEndingTime() const { return MatchEndingTime; }

	// 매치가 종료되는 시간까지 남은 시간을 가져옵니다.
	UFUNCTION(BlueprintGetter)
	float GetMatchRemainTime() const { return MatchEndingTime - GetServerWorldTimeSeconds(); }

	FORCEINLINE bool GetbIsClairvoyanceActivated() const { return bIsClairvoyanceActivated;}
	
	FORCEINLINE ETeam GetClientTeam() const { return ClientTeam; }
	
	FORCEINLINE void SetbIsClairvoyanceActivated(bool NewbIsClairvoyance) { bIsClairvoyanceActivated = NewbIsClairvoyance;}

	virtual void SetClientTeam(const ETeam& NewTeam) {};
	
	UFUNCTION(NetMulticast, Reliable)
	void NotifyPlayerKilled(APlayerState* VictimPlayer, APlayerState* InstigatorPlayer, AActor* DamageCauser);

protected:
	virtual class UGameLobbyCharacterSelectWidget* GetCharacterSelectWidget();

	UFUNCTION()
	virtual void OnRep_MatchEndingTime();

	UFUNCTION()
	virtual void OnRep_CharacterSelectEndingTime();
	
	UFUNCTION()
	virtual void OnRep_MatchWaitEndingTime();

	bool SpawnOutlineManager();

	virtual void ReserveSendRecord();

	virtual bool TrySendMatchResultData();

	virtual void SetScoreBoardVisibility(const bool& Visible);

	virtual void SetTabMinimapVisibility(const bool& Visible);
private:
	void SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime,
	                      const FTimerDelegate& Callback, TWeakObjectPtr<class UGameTimeWidget> TimeWidget);

public:
	virtual bool HasMatchStarted() const override;

	// 현재 접속중인 플레이어 인원이 변경되면 호출됩니다. 매개변수로 변경된 플레이어 인원을 받습니다.
	OnChangePlayerNumberSignature OnChangePlayerNumber;

	//플레이어가 죽은 것이 전달되었을 때 호출됩니다 매개변수로 죽은 플레이어의 컨트롤러, 죽인 플레이어 컨트롤러, 데미지 커서를 받습니다 
	FOnPlayerKillNotifiedSignature OnPlayerKillNotified;

protected:
	int64 StartTimeStamp;	//유닉스 타임 스탬프를 사용합니다
	float StartTime;		//서버가 시작된 시점을 0초로 계산합니다
	
	// 게임이 최대 몇초간 진행될지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchDuration;

	// 캐릭터 선택 시간을 정의합니다.
	UPROPERTY(EditDefaultsOnly)
	float CharacterSelectDuration;
	
	// 게임시작 후 몇초간 대기할 지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchWaitDuration;

	// 플레이가능한 최대 인원 수를 정의합니다.
	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumPlayers;

	UPROPERTY(EditDefaultsOnly)
	float IntroDuration;
	
	// 매치가 몇초간 지속될 지를 정의합니다.
	UPROPERTY(ReplicatedUsing=OnRep_MatchEndingTime)
	float MatchEndingTime;

	// 캐릭터 선택이 몇초간 지속될 지를 정의합니다.
	UPROPERTY(ReplicatedUsing = OnRep_CharacterSelectEndingTime)
	float CharacterSelectEndingTime;
	
	// 게임시작대기가 몇초간 지속될 지를 정의합니다.
	UPROPERTY(ReplicatedUsing=OnRep_MatchWaitEndingTime)
	float MatchWaitEndingTime;

	ETeam ClientTeam;

	FTimerHandle EndingTimer;
	FTimerHandle CharacterSelectTimer;
	FTimerHandle MatchWaitToStartTimer;
	FTimerHandle IntroTimer;

#pragma region Widget
	// Tab키를 눌렀을 때 표시되는 점수판 위젯의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameScoreBoardWidget> ScoreBoardClass;

	// 다른 플레이어의 접속을 대기할 때 표시되는 위젯의 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ULoadingWidget> LoadingWidgetClass;

	// 캐릭터 선택창 위젯의 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameLobbyCharacterSelectWidget> CharacterSelectWidgetClass;

	// 게임중에 표시되는 타이머 위젯 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameTimeWidget> InGameTimerWidgetClass;

	// 캐릭터 선택 중에 표시되는 타이머 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameTimeWidget> CharacterSelectTimerWidgetClass;
	
	// 에임에 있는 플레이어의 이름을 표기해주는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlayerNameDisplayerWidget> PlayerNameDisplayerWidgetClass;

	// 킬로그 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGamePlayKillLogWidget> KillLogWidgetClass;

	// 플레이어의 투시를 표시해주는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AOutlineManager> OutlineManagerClass;

	// 인게임도중 좌측상단에 띄워지는 미니맵 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UOverlayMinimapWidget> HUDMinimapWidgetClass;

	// 인게임 도중 Tab키를 눌렀을 시, 띄워지는 미니맵 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTabMinimapWidget> TabMinimapWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIntroWidget> IntroWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCommonActivatableWidget> InGameWidgetStackClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameStateSequentialWidget> SequentialWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UGameStateSequentialWidget> SequentialWidget;
	
	// 캐릭터 선택 위젯 입니다.	
	TWeakObjectPtr<UGameLobbyCharacterSelectWidget> CharacterSelectWidget;

	// 게임 시작 대기중에 표시되는 위젯입니다.
	TWeakObjectPtr<ULoadingWidget> LoadingWidget;

	// 게임중에 표시되는 점수판 위젯입니다.
	TWeakObjectPtr<UGameScoreBoardWidget> ScoreBoard;

	// 캐릭터 선택중에 표시되는 타이머 위젯입니다.
	TWeakObjectPtr<UGameTimeWidget> CharacterSelectTimeWidget;

	// 게임중에 표시되는 타이머 위젯입니다.
	TWeakObjectPtr<UGameTimeWidget> InGameTimeWidget;

	// 게임중에 표시되는 킬로그 위젯입니다.
	TWeakObjectPtr<UGamePlayKillLogWidget> KillLogWidget;

	// 에임에 있는 플레이어의 이름을 표기해주는 위젯입니다.
	TWeakObjectPtr<UPlayerNameDisplayerWidget> PlayerNameDisplayerWidget;

	// 플레이어의 투시를 표시해주는 위젯입니다.
	TWeakObjectPtr<AOutlineManager> OutlineManager;

	// 인게임도중 좌측상단에 띄워지는 미니맵 위젯입니다.
	TObjectPtr<UOverlayMinimapWidget> HUDMinimapWidget;

	// 인게임 도중 Tab키를 눌렀을 시, 띄워지는 미니맵 위젯입니다.
	TObjectPtr<UTabMinimapWidget> TabMinimapWidget;

	TWeakObjectPtr<UIntroWidget> IntroWidget;

	//인게임 일반 UI를 담는 위젯입니다.
	TObjectPtr<class UCommonActivatableWidget> InGameWidgetStack;

#pragma endregion
	
private:
	bool bWantsSendRecordResult;
	bool bIsClairvoyanceActivated;
};