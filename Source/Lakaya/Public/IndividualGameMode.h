#pragma once

#include "EngineMinimal.h"
#include "LakayaDefalutPlayGameMode.h"
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

	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
public:
	void InitRandomSpawn();
	void SpawnItem();
	void ItemNumCheck();

public:
	void OnKilledCharacter(AController* KilledCharacter, AController* EventInstigator);
private:
	uint8 NumPlayers;
	EGameState GameState;
	
public:
	// TODO : 기획에 따라서 변경될 수 있음.
	const uint8 MaxPlayers = 6;

	// TODO : 기획에 따라서 변경될 수 있음.
	TArray<uint8> VectorArray;
	const uint8 ItemMaxCount = 3;
	const uint8 PosMinCount = 1;
	const uint8 PosMaxCount = 6;
	const int32 PosX = 1000;

	const uint8 PlayerRespawnTime = 1;
	
	const TArray<FVector> ItemPositions = {
		FVector(PosX, 200, 0),
		FVector(PosX, 400, 0),
		FVector(PosX, 600, 0),
		FVector(PosX, 800, 0),
		FVector(PosX, 1000, 0),
		FVector(PosX, 1200, 0)
	};

private:
	FTimerHandle TimerHandle_SpawnItem;
	
};