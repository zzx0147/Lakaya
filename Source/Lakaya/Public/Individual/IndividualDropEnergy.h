#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interactable/Interactable.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	void LocationSetDropEnergy(AController* DeadPlayer);

	void Activate();
	void Deactivate();

public:
	bool IsActive = false;

public:
	FTimerHandle VisibilityTimerHandle;
};