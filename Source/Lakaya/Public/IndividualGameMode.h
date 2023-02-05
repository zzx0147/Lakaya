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

	// void SpawnAndInsertTArray();

public:
	// TArray<AActor*> IndividualItems;
	// int32 CurrentItemCount;
	// int32 MaxCount;
	// int32 MinCount;
};