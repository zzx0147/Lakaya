#pragma once

#include "EngineMinimal.h"
#include "LakayaDefalutPlayGameMode.h"
#include "tiffio.h"
#include "Individual/DropEnergyPool.h"
#include "Misc/LowLevelTestAdapter.h"
#include "IndividualGameMode.generated.h"

UENUM()
enum class EGameState : uint8
{
	StandBy UMETA(DisplayName = "StandBy"), // 다른 플레이어 입장 대기 상태
	SelectWait UMETA(DisplayName = "SelectWait"), // 무기 및 캐릭터 선택 대기 상태
	Progress UMETA(DisplayName = "Progress"), // 게임진행 상태
	Finish UMETA(DisplayName = "Finish") // 게임종료 상태
};

UCLASS()
class LAKAYA_API AIndividualGameMode : public ALakayaDefalutPlayGameMode
{
	GENERATED_BODY()

private:
	AIndividualGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
private:
	UFUNCTION()
	void OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser);
	
	void OnPlayerJoined(APlayerController* PlayerController);

	void RespawnPlayer(AController* Controller);

	void SpawnDropEnergy(AController* DeadPlayer);
	
private:
	ADropEnergyPool* DropEnergyPool;
	
private:
	uint8 NumPlayers;
	EGameState GameState;
	
private:
	// TODO : 기획에 따라서 변경될 수 있음.
	const uint8 MaxPlayers = 6;

	const uint8 PlayerRespawnTime = 3;

private:
	TSet<APlayerController*> RegisteredPlayers;
	UPROPERTY()
	TMap<AController*, FTimerHandle> RespawnTimers;
	
private:
	FTimerHandle TimerHandle_SpawnStaticEnergy;
};