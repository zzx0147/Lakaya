#pragma once

#include "EngineMinimal.h"
#include "LakayaDefalutPlayGameMode.h"
#include "IndividualGameMode.generated.h"

UCLASS()
class LAKAYA_API AIndividualGameMode : public ALakayaDefalutPlayGameMode
{
	GENERATED_BODY()

public:
	AIndividualGameMode();

	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	void InitRandomSpawn();
	void SpawnItem();
	
public:
	TArray<int32> VectorArray;
	const int32 ItemMaxCount = 3;
	const int32 PosMinCount = 1;
	const int32 PosMaxCount = 6;
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
	
private:
	// int32 Minutes;
	// int32 Seconds;

public:
	// int32 GetMinutes();
	// int32 GetSeconds();
	//
	// int32 SetMinutes(int32 Value);
	// int32 SetSeconds(int32 Value);
	//
	// int32 MinusMinutes(int32 Value);
	// int32 MinusSeconds(int32 Value);
};