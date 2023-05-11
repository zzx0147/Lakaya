#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LakayaBaseGameState.generated.h"

DECLARE_EVENT_OneParam(ALakayaBaseGameState,OnChangePlayerNumberSigniture,const uint8&)

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
	virtual void HandleMatchIsCharacterSelect();

public:
	virtual void OnRep_MatchState() override;


public:
	UFUNCTION()
	const uint8& GetMaximumPlayers() const { return MaximumPlayers; }

	UFUNCTION()
	const uint8 GetPlayersNumber() const { return PlayerArray.Num(); }

	// 점수판의 표시 여부를 결정합니다. true를 넘기면 점수판이 표시됩니다.
	void SetScoreBoardVisibility(const bool& Visible);

	virtual void CreateCharacterSelectWidget(APlayerController* LocalController);
protected:

public:
	OnChangePlayerNumberSigniture OnChangePlayerNumber;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameScoreBoardWidget> ScoreBoardClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ULoadingWidget> LoadingWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameLobbyCharacterSelectWidget> CharacterSelectWidgetClass;

	TObjectPtr<UGameLobbyCharacterSelectWidget> CharacterSelectWidget;
private:
	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumPlayers;

	TObjectPtr<ULoadingWidget> LoadingWidget;


	TWeakObjectPtr<UGameScoreBoardWidget> ScoreBoard;
};