#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "IndividualDropEnergy.generated.h"

UCLASS()
class LAKAYA_API AIndividualDropEnergy : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AIndividualDropEnergy();
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void InteractionStart(const float& Time, APawn* Caller) override;
	virtual void InteractionStop(const float& Time, APawn* Caller) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Sphere;
};
