#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/GameState.h"
#include "UI/SkillWidget.h"
#include "UI/MiniMapWidget.h"
#include "functional"
#include "LakayaBaseGameState.generated.h"

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

	// 점수판의 표시 여부를 결정합니다. true를 넘기면 점수판이 표시됩니다.
	void SetScoreBoardVisibility(const bool& Visible);

	UFUNCTION(NetMulticast, Reliable)
	void NotifyPlayerKilled(APlayerState* VictimPlayer, APlayerState* InstigatorPlayer, AActor* DamageCauser);

protected:
	virtual class UGameLobbyCharacterSelectWidget* GetCharacterSelectWidget();

	UFUNCTION()
	virtual void OnRep_MatchEndingTime();

	UFUNCTION()
	virtual void OnRep_MatchWaitEndingTime();

	bool SpawnOutlineManager();

	virtual void ReserveSendRecord();

	virtual bool TrySendMatchResultData();
	
private:
	void SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime,
	                      std::function<void()> Callback, TWeakObjectPtr<class UGameTimeWidget> TimeWidget);

	void InternalSetScoreBoardVisibility(const bool& Visible);

public:
	virtual bool HasMatchStarted() const override;

	// 현재 접속중인 플레이어 인원이 변경되면 호출됩니다. 매개변수로 변경된 플레이어 인원을 받습니다.
	OnChangePlayerNumberSignature OnChangePlayerNumber;

	//플레이어가 죽은 것이 전달되었을 때 호출됩니다 매개변수로 죽은 플레이어의 컨트롤러, 죽인 플레이어 컨트롤러, 데미지 커서를 받습니다 
	FOnPlayerKillNotifiedSignature OnPlayerKillNotified;

protected:
	int64 StartTimeStamp;	//유닉스 타임 스탬프를 사용합니다
	float StartTime;		//서버가 시작된 시점을 0초로 계산합니다

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

	// 게임중에 표시되는 크로스헤어 위젯을 지정합니다.
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<class UGamePlayCrosshairWidget> CrosshairWidgetClass;

	// 게임중에 표시되는 크로스헤어 위젯을 지정합니다.
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<class UDynamicCrossHairWidget> DynamicCrossHairWidgetClass;
	
	// 에임에 있는 플레이어의 이름을 표기해주는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlayerNameDisplayerWidget> PlayerNameDisplayerWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGamePlayKillLogWidget> KillLogWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AOutlineManager> OutlineManagerClass;

	// 미니맵위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMiniMapWidget> MiniMapWidgetClass;
	
	// TODO : 구현되어 있지 않아 주석처리 합니다.
	// 게임중에 표시되는 도움말 위젯을 지정합니다.
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<class UHelpWidget> HelpWidgetClass;
	
	// 게임이 최대 몇초간 진행될지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchDuration;

	// 게임시작 후 몇초간 대기할 지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchWaitDuration;

	// 플레이가능한 최대 인원 수를 정의합니다.
	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumPlayers;

	// 매치가 몇초간 지속될 지를 정의합니다.
	UPROPERTY(ReplicatedUsing=OnRep_MatchEndingTime)
	float MatchEndingTime;

	// 게임시작대기가 몇초간 지속될 지를 정의합니다.
	UPROPERTY(ReplicatedUsing=OnRep_MatchWaitEndingTime)
	float MatchWaitEndingTime;
	
	FTimerHandle EndingTimer;
	FTimerHandle MatchWaitToStartTimer;

	TWeakObjectPtr<UGameLobbyCharacterSelectWidget> CharacterSelectWidget;
	// TWeakObjectPtr<UGamePlayCrosshairWidget> CrosshairWidget;
	// TWeakObjectPtr<UDynamicCrossHairWidget> DynamicCrossHairWidget;
	TWeakObjectPtr<ULoadingWidget> LoadingWidget;
	TWeakObjectPtr<UGameScoreBoardWidget> ScoreBoard;
	TWeakObjectPtr<UGameTimeWidget> InGameTimeWidget;
	TWeakObjectPtr<UGamePlayKillLogWidget> KillLogWidget;
	TWeakObjectPtr<UPlayerNameDisplayerWidget> PlayerNameDisplayerWidget;
	TWeakObjectPtr<AOutlineManager> OutlineManager;
	TWeakObjectPtr<UMiniMapWidget> MiniMapWidget;
	
	// TODO : 아직 구현되어 있지 않아 주석처리합니다.
	// TWeakObjectPtr<UHelpWidget> HelpWidget;
	
	// TODO : 개인전에서는 스킬을 사용하지 않기에 주석처리 해주었습니다.
	// TWeakObjectPtr<USkillWidget> SkillWidget;
	
private:
	bool bWantsSendRecordResult;
};