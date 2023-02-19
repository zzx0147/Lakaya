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
	void RandomSpawn();
	
public:
	TArray<int32> VectorArray;
	int32 ItemMaxCount;
	int32 PosMinCount;
	int32 PosMaxCount;
	
	// TODO
	FVector pos_01;
	FVector pos_02;
	FVector pos_03;
	FVector pos_04;
	FVector pos_05;
	FVector pos_06;

	int32 Minute;
	int32 Second;
};