#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
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
	UCapsuleComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cylinder;
	
public:
	void MakeAvailable();
	
private:
	bool bIsAvailable;

private:
	FTimerHandle AvailableTimerHandle;
};
