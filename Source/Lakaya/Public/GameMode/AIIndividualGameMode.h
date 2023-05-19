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

	// TODO : 캐릭터 사망, 리스폰 리펙토링 후 작업진행
	// virtual void OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser) override;
	
protected:
	// 함수에 대한 설명은 부모클래스에 설명되어 있음.
	virtual void BeginPlay() override;
	// virtual void PostLogin(APlayerController* NewPlayer) override;
	// virtual bool ReadyToStartMatch_Implementation() override;
	// virtual bool ReadyToEndMatch_Implementation() override;
	// virtual void HandleMatchIsSelectCharacter() override;
	virtual void HandleMatchHasStarted() override;
	// virtual void HandleMatchHasEnded() override;

	// TODO : 캐릭터 사망, 리스폰 리펙토링 후 작업진행
	// virtual void RespawnPlayer(AController* KilledController) override;

// protected:
// 	virtual void RespawnPlayer(AController* KilledController) override;
// 	virtual void PlayerInitializeSetLocation(uint8 PlayersNum) override;
// 	
// private:
// 	FTimerHandle TimerHandle_DelayedStart;
// 	FTimerHandle TimerHandle_DelayedEnded;
// 	
// 	class AAIIndividualGameState* AIIndividualGameState;
private:
	UPROPERTY(EditDefaultsOnly)
	int32 NumberOfAi;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAiCharacterController> AIControllerClass;

	TArray<TObjectPtr<AAiCharacterController>> AiControllerArray;
};