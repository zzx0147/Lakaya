#pragma once

#include "CoreMinimal.h"
#include "LakayaBaseGameState.h"
#include "EOS/EOSGameInstance.h"
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

protected:
	virtual void BeginPlay() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;

public:
	// 현재 두 팀의 점수를 기준으로 승자를 정합니다.
	void SetOccupationWinner();

	/**
	 * @brief 팀에 점수를 부여합니다.
	 * @param Team 점수를 부여할 팀입니다.
	 * @param AdditiveScore 추가될 점수입니다.
	 */
	void AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore);

	// 해당 팀의 점수를 받아옵니다.
	float GetTeamScore(const EPlayerTeam& Team) const;

	// 최대 점수를 가져옵니다.
	FORCEINLINE const float& GetMaxScore() const { return MaxScore; }

	// 승리자의 정보를 가져옵니다.
	FORCEINLINE const EPlayerTeam& GetOccupationWinner() const { return CurrentOccupationWinner; }

	// 어떤 팀이든 최대 점수에 도달한 팀이 있는지 여부를 조사합니다.
	FORCEINLINE bool GetSomeoneReachedMaxScore() const
	{
		return ATeamScore >= MaxScore || BTeamScore >= MaxScore;
	}

	void EndTimeCheck();

	void ChangeResultWidget();

protected:
	virtual void SetClientTeam(const EPlayerTeam& NewTeam);
	virtual bool TrySendMatchResultData() override;
private:
	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();

	void DestroyShieldWallObject();

	// 게임 승패여부를 띄워줍니다.
	void ShowEndResultWidget();

	// 게임결과 배경위젯를 띄워줍니다.
	void ShowGradeResultWidget(class ALakayaBasePlayerState* PlayerState, APlayerController* Controller);

	// 게임결과 등수 위젯을 띄워줍니다.
	void ShowGradeResultElementWidget(ALakayaBasePlayerState* NewPlayerState) const;

	void GradeResultTeamInfo(TArray<TObjectPtr<ALakayaBasePlayerState>>& PlayerArray, uint8 NewIndex) const;

	// 본인의 팀에 따라 보여지는 게임결과 등수 위젯을 띄워줍니다.
	void ShowAntiTeamGradeResultElementWidget() const;
	void ShowProTeamGradeResultElementWidget() const;

	// DetailResultWidget에서 본인의 정보를 바인딩합니다.
	void BindDetailResultWidget();

	// 모든 인원들의 정보를 담은 위젯을 바인딩합니다.
	void BindDetailResultElementWidget();

	FORCEINLINE TArray<TObjectPtr<ALakayaBasePlayerState>>& GetAllyArray()
	{
		return PlayersByTeamMap[ClientTeam];
	}

	FORCEINLINE TArray<TObjectPtr<ALakayaBasePlayerState>>& GetEnemyArray()
	{
		return PlayersByTeamMap[ClientTeam == EPlayerTeam::A ? EPlayerTeam::B : EPlayerTeam::A];
	}

	void UpdatePlayerByTeamMap(const EPlayerTeam& Team, ALakayaBasePlayerState* PlayerState);
	void SetupPlayerStateOnLocal(ALakayaBasePlayerState* PlayerState);
	static ERendererStencilMask GetUniqueStencilMask(const bool& IsAlly, const uint8& Index);
	void OnPlayerStateOwnerChanged(AActor* Owner);

	void AddPlayerStateToRecordResult(EPlayerTeam InTeam ,TArray<ALakayaBasePlayerState*> InPlayers);
	
public:
	FOnChangeOccupationWinner OnChangeOccupationWinner;
	FTeamScoreSignature OnTeamScoreSignature;
	

private:
	
	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner, Transient)
	EPlayerTeam CurrentOccupationWinner;

	// Anti
	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore, Transient)
	float ATeamScore;

	// Pro
	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore, Transient)
	float BTeamScore;

	UPROPERTY(EditAnywhere)
	float MaxScore;

	UPROPERTY(EditAnywhere)
	float MatchStartWaitWidgetLifeTime;

	UPROPERTY(EditAnywhere)
	float MatchStartWidgetLifeTime;

	bool ResultBool = false;

	TMap<EPlayerTeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> PlayersByTeamMap;

	EPlayerTeam ClientTeam;

	FTimerHandle TimerHandle_GameTimeCheck;
	FTimerHandle TimerHandle_StartMessageVisible;
	FTimerHandle TimerHandle_StartMessageHidden;
	FTimerHandle TimerHandle_WaitTimerHandle;
	FTimerHandle TimerHandle_GameResultHandle;
	FTimerHandle TimerHandle_ShowGradeResultElementHandle;

	// 게임중에 표시되는 스킬 위젯을 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USkillWidget> SkillWidgetClass;

	// 게임중에 표시되는 팀 스코어 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTeamScoreWidget> TeamScoreWidgetClass;

	// 게임 시작 시 무기박스 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWeaponOutLineWidget> WeaponOutLineWidgetClass;
	
	// 게임 시작 시 "라카야 제어기를 점령하세요" 메세지를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UStartMessageWidget> StartMessageWidgetClass;

	// 게임 시작 시 "라운드 시작까지 10초 남았습니다" 메세지를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMatchStartWaitWidget> MatchStartWaitWidgetClass;
	
	// 게임 종료 시 "승리", "패배" 및 팀별 점수를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameResultWidget> GameResultWidgetClass;
	
	// 게임 종료 후 배경 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGradeResultWidget> GradeResultWidgetClass;

	// 게임 종료 후 게임 팀내 등수 결과를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGradeResultElementWidget> GradeResultElementWidgetClass;

	// 게임 종료 시 게임 디테일 결과를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDetailResultWidget> DetailResultWidgetClass;

	// 게임 종료 시 게임 디테일 결과에서 플레이어들의 정보를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDetailResultElementWidget> DetailResultElementWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* ResultShortcutContext;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* ResultSwitchingAction;

	// 스킬 위젯 입니다.
	TWeakObjectPtr<USkillWidget> SkillWidget;

	// 팀스코어 위젯 입니다.
	TObjectPtr<UTeamScoreWidget> TeamScoreWidget;

	// 무기박스 위젯 입니다.
	TObjectPtr<UWeaponOutLineWidget> WeaponOutLineWidget;
	
	// "라운드 시작까지 10초 남았습니다" 위젯 입니다.
	TWeakObjectPtr<UMatchStartWaitWidget> MatchStartWaitWidget;

	// "라카야 제어기를 점령하세요" 위젯 입니다.
	TWeakObjectPtr<UStartMessageWidget> StartMessageWidget;

	// 게임 승패 위젯 입니다.
	TWeakObjectPtr<UGameResultWidget> GameResultWidget;

	// 게임 종료 후 배경 위젯입니다.
	TWeakObjectPtr<UGradeResultWidget> GradeResultWidget;

	// 게임 팀내 등수 결과 위젯입니다.
	TWeakObjectPtr<UGradeResultElementWidget> GradeResultElementWidget;

	// 게임 디테일 결과 위젯입니다.
	TWeakObjectPtr<UDetailResultWidget> DetailResultWidget;

	// 게임 디테일 Element 결과 위젯입니다.
	TWeakObjectPtr<UDetailResultElementWidget> DetailResultElementWidget;

	FMatchResultStruct MatchResult;
	
public:
	bool Tapbool = true;
};
