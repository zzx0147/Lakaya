#pragma once

#include "CoreMinimal.h"
#include "LakayaBaseGameState.h"
#include "EOS/EOSGameInstance.h"
#include "Occupation/Team.h"
#include "OccupationGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeOccupationWinner, const ETeam&)
DECLARE_EVENT_TwoParams(AOccupationGameState, FTeamScoreSignature, const ETeam&, const float&)

UCLASS()
class LAKAYA_API AOccupationGameState : public ALakayaBaseGameState
{
	GENERATED_BODY()

public:
	AOccupationGameState();

	/**
	 * @brief 게임이 끝났을 때, 현재 두 팀의 점수를 비교하여 승자를 결정하는 함수입니다.
	 */
	void SetOccupationWinner();

	void ChangeResultWidget();

	/**
	* @brief 점령에 성공한 팀이 상대편의 점령구역 개수가 많은지 판별합니다.
	* @param Team 점령에 성공한 팀입니다.
	*/
	UFUNCTION()
	bool CheckCaptureAreaCount(const ETeam& Team);

	/**
	 * @brief 팀에서 점령중인 점령구역 개수를 추가해주는 함수입니다.
	 * @param Team 점령구역 개수를 늘린 팀입니다.
	 */
	void AddCaptureAreaCount(const ETeam& Team);

	/**
	 * @brief 팀에서 점령중인 점령구역 개수를 감소해주는 함수입니다.
	 * @param Team 점령구역 개수를 줄일 팀입니다.
	 */
	void SubCaptureAreaCount(const ETeam& Team);

	/**
	 * @brief 스코어 업데이트 타이머를 시작하는 함수입니다.
	 * @param Team 우세한 팀입니다.
	 * @param UpdateDelay 딜레이입니다.
	 */
	void StartScoreUpdate(const ETeam& Team, float UpdateDelay);

	/**
	 * @brief 타이머를 중지시키는 함수입니다.
	 */
	void StopScoreUpdate();
	
	FORCEINLINE const float& GetTeamScore(const ETeam& Team) const { return (Team == ETeam::Anti) ? AntiTeamScore : ProTeamScore; }
	FORCEINLINE const float& GetMaxScore() const { return MaxScore; }
	FORCEINLINE const ETeam& GetOccupationWinner() const { return CurrentOccupationWinner; }
	FORCEINLINE bool GetSomeoneReachedMaxScore() const { return AntiTeamScore >= MaxScore || ProTeamScore >= MaxScore; }
	FORCEINLINE const uint8& GetAntiTeamCaptureAreaCount() const { return AntiTeamCaptureAreaCount; }
	FORCEINLINE const uint8& GetProTeamCaptureAreaCount() const { return ProTeamCaptureAreaCount; }
	FORCEINLINE TArray<TObjectPtr<ALakayaBasePlayerState>>& GetAllyArray() { return PlayersByTeamMap[ClientTeam]; }
	FORCEINLINE TArray<TObjectPtr<ALakayaBasePlayerState>>& GetEnemyArray() { return PlayersByTeamMap[ClientTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti]; }
	FORCEINLINE const ETeam& GetTeamToUpdate() const { return TeamToUpdate; }
	
	FORCEINLINE void SetAntiTeamCaptureAreaCount(const uint8& NewCaptureCount) { AntiTeamCaptureAreaCount = NewCaptureCount; }
	FORCEINLINE void SetProTeamCaptureAreaCount(const uint8& NewCaptureCount) { ProTeamCaptureAreaCount = NewCaptureCount; }
	FORCEINLINE void SetTeamToUpdate(const ETeam& NewTeam) { TeamToUpdate = NewTeam; }
protected:
	virtual void BeginPlay() override;

	/**
	 * @brief 게임 내에서 매치가 시작했을 때, 호출되는 함수입니다.
	 */
	virtual void HandleMatchHasStarted() override;

	/**
	 * @brief 게임 내에서 매치가 끝났을 때, 호출되는 함수입니다.
	 */
	virtual void HandleMatchHasEnded() override;

	/**
	 * @brief 게임에 참가중인 플레이 리스트에 새로 들어온 참가자를 추가하는 함수입니다.
	 * @param PlayerState 새로 참가한 플레이어의 PlayerState 정보입니다.
	 */
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	
	virtual bool TrySendMatchResultData() override;
	
private:
	virtual void SetClientTeam(const ETeam& NewTeam);
	
	void EndTimeCheck();
	
	void DestroyShieldWallObject();

	void UpdatePlayerByTeamMap(const ETeam& Team, ALakayaBasePlayerState* PlayerState);

	void SetupPlayerStateOnLocal(ALakayaBasePlayerState* PlayerState);

	static ERendererStencilMask GetUniqueStencilMask(const bool& IsAlly, const uint8& Index);

	void OnPlayerStateOwnerChanged(AActor* Owner);

	void AddPlayerStateToRecordResult(ETeam InTeam ,TArray<ALakayaBasePlayerState*> InPlayers);

	/**
	 * @brief 스코어를 업데이트 해주는 함수입니다.
	 */
	void UpdateTeamScoreTick();

#pragma region Widget
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
#pragma endregion

#pragma region OnRep
	UFUNCTION()
	void OnRep_AntiTeamScore();

	UFUNCTION()
	void OnRep_ProTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();
#pragma endregion
public:
	FOnChangeOccupationWinner OnChangeOccupationWinner;
	FTeamScoreSignature OnTeamScoreSignature;
	
	bool bTap;
private:
	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner, Transient)
	ETeam CurrentOccupationWinner;

	UPROPERTY(ReplicatedUsing = OnRep_AntiTeamScore, Transient)
	float AntiTeamScore;

	UPROPERTY(ReplicatedUsing = OnRep_ProTeamScore, Transient)
	float ProTeamScore;

	UPROPERTY(Replicated)
	uint8 AntiTeamCaptureAreaCount;

	UPROPERTY(Replicated)
	uint8 ProTeamCaptureAreaCount;
	
	UPROPERTY(EditAnywhere)
	float MaxScore;

	UPROPERTY(EditAnywhere)
	float ScoreUpdateDelay;

	UPROPERTY(EditAnywhere)
	float AdditiveScore;
	
	UPROPERTY(EditAnywhere)
	float MatchStartWaitWidgetLifeTime;

	UPROPERTY(EditAnywhere)
	float MatchStartWidgetLifeTime;

	bool ResultBool = false;

	TMap<ETeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> PlayersByTeamMap;

	ETeam ClientTeam;

	UPROPERTY(Replicated)
	ETeam TeamToUpdate = ETeam::None;
	
	FMatchResultStruct MatchResult;

	FTimerHandle TimerHandle_GameTimeCheck;
	FTimerHandle TimerHandle_StartMessageVisible;
	FTimerHandle TimerHandle_StartMessageHidden;
	FTimerHandle TimerHandle_WaitTimerHandle;
	FTimerHandle TimerHandle_GameResultHandle;
	FTimerHandle TimerHandle_ShowGradeResultElementHandle;
	FTimerHandle TimerHandle_UpdateScoreTimer;
#pragma region Widget
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
#pragma endregion
};