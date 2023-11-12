#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "UI/IndividualWidget/IndividualBaseWidget.h"
#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"
#include "AIIndividualGameState.generated.h"

UCLASS()
class LAKAYA_API AAIIndividualGameState : public ALakayaBaseGameState
{
	GENERATED_BODY()

public:
	AAIIndividualGameState();

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void SetScoreBoardVisibility(const bool& Visible) override;
	virtual void SetTabMinimapVisibility(const bool& Visible) override;
	void SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray) const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;

private:
	ERendererStencilMask GetUniqueStencilMaskWithCount(const uint8& Count);
	void InternalSetScoreBoardVisibility(const bool& Visible) const;
	void InternalSetTabMinimapVisibility(const bool& Visible) const;

	/**
	 * @brief EnemiesByMinimap을 업데이트 해주는 함수입니다.
	 * @param NewPlayerState 업데이트시켜줄 PlayerState입니다.
	 * @param NewPlayerImage 업데이트 된 플레이어의 미니맵 이미지입니다.
	 */
	void UpdatePlayerByMinimap(const ALakayaBasePlayerState* NewPlayerState, const UImage* NewPlayerImage);

	virtual void SetClientTeam(const ETeam& NewTeam) override;
public:
	TArray<FPlayerAIData> FPlayerAIDataArray;

	FPlayerAIData PlayerAIData;

	TArray<TWeakObjectPtr<ALakayaBasePlayerState>> PlayerArrays;
	
private:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIndividualGameResultWidget> GameResultWidgetClass;

	// 게임 종료 시 등수를 뛰우는 위젯 클래스를 지정합니다.
	TWeakObjectPtr<UIndividualGameResultWidget> GameResultWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardWidgetClass;

	// 게임 종료 시 게임 디테일 결과를 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UAIIndividualFinalResultWidget> AIIndividualFinalResultWidgetClass;
	
	// 게임 종료 후 게임 디테일 결과 위젯입니다.
	TObjectPtr<UAIIndividualFinalResultWidget> AIIndividualFinalResultWidget;

	// 게임 종료 시 "1등", "2등", "N둥"..위젯을 띄우는 위젯 클래스를 지정합니다.
	TWeakObjectPtr<class UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardWidget;

	UPROPERTY(EditAnywhere)
	float MatchStartWaitWidgetLifeTime;

	UPROPERTY(EditAnywhere)
	float AIIndividualFinalResultLifeTime;
	
	FString AIName;

	FTimerHandle TimerHandle_StartMessageVisible;
	FTimerHandle TimerHandle_StartMessageHidden;
	FTimerHandle TimerHandle_WaitTimerHandle;
	FTimerHandle TimerHandle_AIIndividualFinalResultHandle;
	FTimerHandle TimerHandle_CharacterSelectTimer;
};
