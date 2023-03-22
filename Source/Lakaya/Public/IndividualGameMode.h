#pragma once

#include "EngineMinimal.h"
#include "LakayaDefalutPlayGameMode.h"
#include "DropEnergyPool.h"
#include "IndividualGameMode.generated.h"

UENUM()
enum class EGameState : uint8
{
	Wait UMETA(DisplayName = "Wait"),
	Progress UMETA(DisplayName = "In Progress"),
	Finish UMETA(DisplayName = "Exit")
};

UCLASS()
class LAKAYA_API AIndividualGameMode : public ALakayaDefalutPlayGameMode
{
	GENERATED_BODY()

public:
	AIndividualGameMode();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	
public:
	UFUNCTION()
	void OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser);
	
	void OnPlayerJoined(APlayerController* PlayerController);
	void RespawnPlayer(AController* Controller);
	
	void SpawnStaticEnergyAtRandomPosition();
	void SpawnStaticEnergy();
	void StaticEnergyNumCheck();

	void SpawnDropEnergy(AController* DeadPlayer);

private:
	ADropEnergyPool* DropEnergyPool;
	
private:
	uint8 NumPlayers;
	EGameState GameState;
	
public:
	// TODO : 기획에 따라서 변경될 수 있음.
	const uint8 MaxPlayers = 6;

	// TODO : 기획에 따라서 변경될 수 있음.
	TArray<uint8> VectorArray;
	const uint8 StaticEnergyMaxCount = 3;
	const uint8 PosMinCount = 1;
	const uint8 PosMaxCount = 6;
	const int32 PosX = 1000;

	const uint8 PlayerRespawnTime = 3;
	
	const TArray<FVector> StaticEnergyPositions = {
		FVector(PosX, 200, 0),
		FVector(PosX, 400, 0),
		FVector(PosX, 600, 0),
		FVector(PosX, 800, 0),
		FVector(PosX, 1000, 0),
		FVector(PosX, 1200, 0)
	};

private:
	TSet<APlayerController*> RegisteredPlayers;
	
private:
	FTimerHandle TimerHandle_SpawnStaticEnergy;
	
	UPROPERTY()
	TMap<AController*, FTimerHandle> RespawnTimers;
};