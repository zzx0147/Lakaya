#pragma once

#include "CoreMinimal.h"
#include "IndividualDropEnergy.h"
#include "GameFramework/Actor.h"
#include "DropEnergyPool.generated.h"

UCLASS()
class LAKAYA_API ADropEnergyPool : public AActor
{
	GENERATED_BODY()
	
public:	
	ADropEnergyPool();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
//
// public:
// 	void Initialize(int32 PoolSize);
// 	AIndividualDropEnergy* GetDropEnergy();
// 	void ReturnDropEnergy(AIndividualDropEnergy* DropEnergy);
//
// public:
// 	UPROPERTY(EditAnywhere)
// 	TSubclassOf<AIndividualDropEnergy> DropEnergyClass;
// 	
// 	UPROPERTY(EditAnywhere)
// 	TArray<AIndividualDropEnergy*> InactiveDropEnergys;
//
// private:
// 	float ElapsedTime = 0.0f;
// 	bool InitPool = false;
};