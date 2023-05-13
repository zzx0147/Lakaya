#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LakayaBaseGameState.generated.h"

DECLARE_EVENT_OneParam(ALakayaBaseGameState, OnChangePlayerNumberSignature, const uint8&)

UCLASS()
class LAKAYA_API ALakayaBaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	ALakayaBaseGameState();

protected:
	//게임 상태가 IsWatingToStart로 넘어가면 호출됨
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void HandleMatchIsCharacterSelect();

public:
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

	virtual void CreateCharacterSelectWidget(APlayerController* LocalController);

	virtual void SetCharacterSelectWidgetVisibility(const ESlateVisibility& IsVisible);

	ESlateVisibility GetCharacterSelectWidgetVisibility() const;

	UFUNCTION(NetMulticast, Reliable)
	virtual void NotifyPlayerKilled(AController* VictimController, AActor* Victim,AController* InstigatorController, AActor* DamageCauser);

protected:
	UFUNCTION()
	virtual void OnRep_MatchEndingTime();

	UFUNCTION()
	virtual void OnRep_CharacterSelectEndingTime();

private:
	void SetupTimerWidget(FTimerHandle& TimerHandle,const float& Duration,float& EndingTime, std::function<void(void)> Callback, TWeakObjectPtr<class UGameTimeWidget> TimeWidget);

private:
	void InternalSetScoreBoardVisibility(const bool& Visible);

public:
	// 현재 접속중인 플레이어 인원이 변경되면 호출됩니다. 매개변수로 변경된 플레이어 인원을 받습니다.
	OnChangePlayerNumberSignature OnChangePlayerNumber;

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
	TSubclassOf<class UGameTimeWidget> InGameTimerWidgetClass;

	// 게임중에 표시되는 타이머 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameTimeWidget> CharacterSelectTimerWidgetClass;

	// 게임이 최대 몇초간 진행될지 정의합니다.
	UPROPERTY(EditAnywhere)
	float MatchDuration;

	UPROPERTY(EditDefaultsOnly)
	float CharacterSelectDuration;

	TObjectPtr<UGameLobbyCharacterSelectWidget> CharacterSelectWidget;

private:
	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_MatchEndingTime)
	float MatchEndingTime;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterSelectEndingTime)
	float CharacterSelectEndingTime;


	FTimerHandle EndingTimer;

	FTimerHandle CharacterSelectTimer;

	TObjectPtr<ULoadingWidget> LoadingWidget;


	TWeakObjectPtr<UGameScoreBoardWidget> ScoreBoard;
	TWeakObjectPtr<UGameTimeWidget> InGameTimeWidget;
	TWeakObjectPtr<UGameTimeWidget> CharacterSelectTimeWidget;
};
