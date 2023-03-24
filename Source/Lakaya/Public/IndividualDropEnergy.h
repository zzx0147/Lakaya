#pragma once

#include "CoreMinimal.h"
#include "IndividualEnergy.h"
#include "IndividualDropEnergy.generated.h"


UCLASS()
class LAKAYA_API AIndividualDropEnergy : public AIndividualEnergy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void InteractionStart(const float& Time, APawn* Caller) override;
	virtual void InteractionStop(const float& Time, APawn* Caller) override;
};
