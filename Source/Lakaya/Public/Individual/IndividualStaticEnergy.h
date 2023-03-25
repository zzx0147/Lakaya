#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interactable/Interactable.h"
#include "IndividualStaticEnergy.generated.h"

UCLASS()
class LAKAYA_API AIndividualStaticEnergy : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AIndividualStaticEnergy();
	
protected:
	virtual auto BeginPlay() -> void override;
	virtual void PostInitializeComponents() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	virtual void OnLocalInteractionBegin(APawn* Caller) override;
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Box;
	
public:
	void MakeAvailable();
	
private:
	bool bIsAvailable;

private:
	FTimerHandle AvailableTimerHandle;
};
