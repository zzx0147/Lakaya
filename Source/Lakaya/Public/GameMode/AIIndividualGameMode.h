#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "AIIndividualGameMode.generated.h"

UCLASS()
class LAKAYA_API AAIIndividualGameMode : public ALakayaDefaultPlayGameMode
{
	GENERATED_BODY()

public:
	AAIIndividualGameMode();

protected:
	// 함수에 대한 설명은 부모클래스에 설명되어 있음.
	virtual void BeginPlay() override;
	// virtual void Tick(float DeltaSeconds) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	// virtual bool ReadyToStartMatch_Implementation() override;
	// virtual bool ReadyToEndMatch_Implementation() override;
	// virtual void HandleMatchIsSelectCharacter() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;

	void HandleKillCountChanged(const uint16& NewKillCount);
	void EndGame(AController* WinerController);

private:
	UPROPERTY(EditDefaultsOnly)
	int32 NumberOfAi;
	
	UPROPERTY(EditDefaultsOnly)
	int32 TargetKills;
	
	FTimerHandle UpdateScoreTimer;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAiCharacterController> AIControllerClass;

	TArray<TObjectPtr<AAiCharacterController>> AiControllerArray;

	AController* PlayerController;
	AController* WinningCharController;
};