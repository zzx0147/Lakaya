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

<<<<<<< HEAD
<<<<<<< HEAD
public:

public:
	TArray<AActor*> ItemArray;
	int32 CurrentCount;
	int32 MaxCount;
	int32 MinCount;

	FVector SpawnLocation;
=======
	void SpawnAndInsertTArray();

public:
=======
	void SpawnAndInsertTArray();

public:
>>>>>>> parent of 06ac0ca (20230206_0056)
	TArray<AActor*> IndividualItems;
	int32 CurrentItemCount;
	int32 MaxCount;
	int32 MinCount;
<<<<<<< HEAD
>>>>>>> parent of 06ac0ca (20230206_0056)
=======
>>>>>>> parent of 06ac0ca (20230206_0056)
};