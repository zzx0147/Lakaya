#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "IndividualObject.generated.h"

UCLASS()
class LAKAYA_API AIndividualObject : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AIndividualObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	virtual void OnLocalInteractionBegin(APawn* Caller) override;
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;
	virtual void OnLocalInteractionStopBegin(APawn* Caller) override;
	virtual void OnServerInteractionStopBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(APawn* Caller) override;
	
	void AutomaticInteractionStop();

	void MakeAvailable();
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCapsuleComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cylinder;

private:
	const float MaxInteractionDuration = 4;
	float InteractingStartTime;
	float InteractingStopTime;

	bool bIsAvailable;
	
	FTimerHandle InteractionTimerHandle;
	FTimerHandle AvailableTimerHandle;
};