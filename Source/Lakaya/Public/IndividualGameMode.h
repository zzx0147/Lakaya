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
	void OnKillCharacter(AController* EventInstigator, AActor* DamageCauser);

private:
	uint8 NumPlayers;
	EGameState GameState;
	
public:
	const uint8 MaxPlayers = 6;
	
	TArray<uint8> VectorArray;
	const uint8 ItemMaxCount = 3;
	const uint8 PosMinCount = 1;
	const uint8 PosMaxCount = 6;
	const int32 PosX = 1000;
	
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