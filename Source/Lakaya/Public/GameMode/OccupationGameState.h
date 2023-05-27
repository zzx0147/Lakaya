#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "Occupation/PlayerTeam.h"
#include "OccupationGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeOccupationWinner, const EPlayerTeam&)
DECLARE_EVENT_TwoParams(AOccupationGameState, FTeamScoreSignature, const EPlayerTeam&, const float&)

UCLASS()
class LAKAYA_API AOccupationGameState : public ALakayaBaseGameState
{
	GENERATED_BODY()

public:
	AOccupationGameState();

private:
	virtual void BeginPlay() override;
	virtual void HandleMatchHasStarted() override;
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void NotifyKillCharacter(AController* KilledController, AActor* KilledActor, AController* EventInstigator,
	                         AActor* Causer);

	// 현재 두 팀의 점수를 기준으로 승자를 정합니다.
	void SetOccupationWinner();

	/**
	 * @brief 팀에 점수를 부여합니다.
	 * @param Team 점수를 부여할 팀입니다.
	 * @param AdditiveScore 추가될 점수입니다.
	 */
	void AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore);

protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;

public:
	// 해당 팀의 점수를 받아옵니다.
	float GetTeamScore(const EPlayerTeam& Team) const;

	// 최대 점수를 가져옵니다.
	FORCEINLINE const float& GetMaxScore() const { return MaxScore; }

	// 승리자의 정보를 가져옵니다.
	FORCEINLINE const EPlayerTeam& GetOccupationWinner() const { return CurrentOccupationWinner; }

	// 어떤 팀이든 최대 점수에 도달한 팀이 있는지 여부를 조사합니다.
	FORCEINLINE const bool GetSomeoneReachedMaxScore() const { return ATeamScore >= MaxScore || BTeamScore >= MaxScore; }

	virtual void CreateCharacterSelectWidget(APlayerController* LocalController) override;

	void EndTimeCheck();
	
private:
	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();

	void SetClientTeam(const EPlayerTeam& NewTeam);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner)
	EPlayerTeam CurrentOccupationWinner = EPlayerTeam::None;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore)
	float ATeamScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore)
	float BTeamScore = 0;

	UPROPERTY(EditAnywhere)
	float MaxScore;

	UPROPERTY(EditAnywhere)
	float MatchStartWaitWidgetLifeTime;

	UPROPERTY(EditAnywhere)
	float MatchStartWidgetLifeTime;
	
	TMap<EPlayerTeam,TArray<class ALakayaBasePlayerState*>> PlayersByTeamMap;

	EPlayerTeam ClientTeam;

	FTimerHandle TimerHandle_GameTimeCheck;
	FTimerHandle TimerHandle_StartMessageVisible;
	FTimerHandle TimerHandle_StartMessageHidden;
	FTimerHandle TimerHandle_WaitTimerHandle;
private:
	// 게임중에 표시되는 팀 스코어 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTeamScoreWidget> TeamScoreWidgetClass;
	
	// 게임 종료 시 승리자를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameResultWidget> GameResultWidgetClass;

	// 게임 시작 시 "라카야 제어기를 점령하세요" 메세지를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UStartMessageWidget> StartMessageWidgetClass;

	// 게임 시작 시 "라운드 시작까지 10초 남았습니다" 메세지를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMatchStartWaitWidget> MatchStartWaitWidgetClass;
	
	// 팀스코어 위젯 입니다.
    TObjectPtr<UTeamScoreWidget> TeamScoreWidget;
	
	// 게임결과창 위젯 입니다.
	TWeakObjectPtr<UGameResultWidget> GameResultWidget;

	// "라카야 제어기를 점령하세요" 위젯 입니다.
	TWeakObjectPtr<UStartMessageWidget> StartMessageWidget;

	// "라운드 시작까지 10초 남았습니다" 위젯 입니다.
	TWeakObjectPtr<UMatchStartWaitWidget> MatchStartWaitWidget;
	
public:
	FOnChangeOccupationWinner OnChangeOccupationWinner;
	FTeamScoreSignature OnTeamScoreSignature;
};

