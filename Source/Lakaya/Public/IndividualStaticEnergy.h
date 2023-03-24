#pragma once

#include "CoreMinimal.h"
#include "IndividualEnergy.h"
#include "IndividualStaticEnergy.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AIndividualStaticEnergy : public AIndividualEnergy
{
	GENERATED_BODY()

protected:
	virtual auto BeginPlay() -> void override;
	virtual void PostInitializeComponents() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	
private:
	virtual void InteractionStart(const float& Time, APawn* Caller) override;
	virtual void InteractionStop(const float& Time, APawn* Caller) override;
};
