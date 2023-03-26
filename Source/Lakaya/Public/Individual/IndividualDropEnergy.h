#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Components/SphereComponent.h"
#include "IndividualDropEnergy.generated.h"

UCLASS()
class LAKAYA_API AIndividualDropEnergy : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AIndividualDropEnergy();
	
protected:
	virtual void BeginPlay() override;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void OnLocalInteractionBegin(APawn* Caller) override;
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Sphere;
	
public:
	void SetDropEnergy(AController* DeadPlayer);

	void Activate();
	void Deactivate();

public:
	bool IsActive;

public:
	FTimerHandle VisibilityTimerHandle;
};