#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "UI/IndividualWidget/IndividualBaseWidget.h"
#include "AIIndividualGameState.generated.h"

UCLASS()
class LAKAYA_API AAIIndividualGameState : public ALakayaBaseGameState
{
	GENERATED_BODY()

public:
	AAIIndividualGameState();

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	void SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray);
	void SetAIIndividualWinner();

	TArray<FPlayerAIData> FPlayerAIDataArray;

	FPlayerAIData PlayerAIData;

	TArray<TWeakObjectPtr<ALakayaBasePlayerState>> AllPlayersArray;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void HandleMatchHasEnded() override;

	TWeakObjectPtr<class UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardWidget;

	// 게임 종료 시 "1등", "2등", "N둥"..위젯을 띄우는 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIndividualGameResultWidget> GameResultWidgetClass;

private:
	ERendererStencilMask GetUniqueStencilMaskWithCount(const uint8& Count);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardWidgetClass;

	// 게임 종료 시 등수를 뛰우는 위젯입니다.
	TWeakObjectPtr<UIndividualGameResultWidget> GameResultWidget;
};
