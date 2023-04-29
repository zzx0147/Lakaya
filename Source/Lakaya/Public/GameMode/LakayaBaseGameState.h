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


public:
	UFUNCTION()
	const uint8& GetMaximumPlayers() const { return MaximumPlayers; }

	UFUNCTION()
	const uint8& GetPlayersNumber() const { return PlayersNumber; }

public:
	OnChangePlayerNumberSigniture OnChangePlayerNumber;

private:
	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumPlayers;
	
	uint8 PlayersNumber;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ULoadingWidget> LoadingWidgetClass;
	TObjectPtr<ULoadingWidget> LoadingWidget;
};