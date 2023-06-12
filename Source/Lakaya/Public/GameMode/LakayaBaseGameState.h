#pragma once

#include "CoreMinimal.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/GameState.h"
#include "UI/SkillWidget.h"
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
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void HandleMatchIsCharacterSelect();
	
	virtual void OnRep_MatchState() override;

public:
	UFUNCTION(BlueprintGetter)
	const uint8& GetMaximumPlayers() const { return MaximumPlayers; }

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
	virtual void OnRep_CharacterSelectEndingTime();

	UFUNCTION()
	virtual void OnRep_MatchWaitEndingTime();

	bool SpawnOutlineManager();

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
	// Tab키를 눌렀을 때 표시되는 점수판 위젯의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameScoreBoardWidget> ScoreBoardClass;

	// 다른 플레이어의 접속을 대기할 때 표시되는 위젯의 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ULoadingWidget> LoadingWidgetClass;

	// 캐릭터 선택창 위젯의 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameLobbyCharacterSelectWidget> CharacterSelectWidgetClass;

	// 게임중에 표시되는 타이머 위젯 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameTimeWidget> InGameTimerWidgetClass;

	// 캐릭터 선택 중에 표시되는 타이머 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameTimeWidget> CharacterSelectTimerWidgetClass;

	// 게임중에 표시되는 크로스헤어 위젯을 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGamePlayCrosshairWidget> CrosshairWidgetClass;

	// 게임중에 표시되는 도움말 위젯을 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UHelpWidget> HelpWidgetClass;

	// TODO : 개인전엣는 스킬을 사용하지 않기에 주석처리 해주었습니다.
	// 게임중에 표시되는 스킬 위젯을 지정합니다.
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<USkillWidget> SkillWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGamePlayKillLogWidget> KillLogWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AOutlineManager> OutlineManagerClass;

	// 게임이 최대 몇초간 진행될지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchDuration;

	UPROPERTY(EditDefaultsOnly)
	float CharacterSelectDuration;

	// 게임시작 후 몇초간 대기할 지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchWaitDuration;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_MatchEndingTime)
	float MatchEndingTime;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterSelectEndingTime)
	float CharacterSelectEndingTime;

	UPROPERTY(ReplicatedUsing=OnRep_MatchWaitEndingTime)
	float MatchWaitEndingTime;

	FTimerHandle EndingTimer;
	FTimerHandle CharacterSelectTimer;
	FTimerHandle MatchWaitToStartTimer;

	TObjectPtr<ULoadingWidget> LoadingWidget;
	TObjectPtr<UGameLobbyCharacterSelectWidget> CharacterSelectWidget;
	TObjectPtr<UGamePlayCrosshairWidget> CrosshairWidget;

	TWeakObjectPtr<UGameScoreBoardWidget> ScoreBoard;
	TWeakObjectPtr<UGameTimeWidget> CharacterSelectTimeWidget;
	TWeakObjectPtr<UGameTimeWidget> InGameTimeWidget;
	TWeakObjectPtr<UHelpWidget> HelpWidget;
	TWeakObjectPtr<UGamePlayKillLogWidget> KillLogWidget;

	// TODO : 개인전엣는 스킬을 사용하지 않기에 주석처리 해주었습니다.
	// TWeakObjectPtr<USkillWidget> SkillWidget;

	TWeakObjectPtr<AOutlineManager> OutlineManager;
};
