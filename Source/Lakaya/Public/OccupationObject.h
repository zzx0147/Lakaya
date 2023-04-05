// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable/Interactable.h"
#include "OccupationObject.generated.h"

UCLASS()
class LAKAYA_API AOccupationObject : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AOccupationObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;
	virtual void OnLocalInteractionStopBegin(APawn* Caller) override;
	virtual void OnServerInteractionStopBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(APawn* Caller) override;

	void AutomaticInteractionStop();

private:
	const float MaxInteractionDuration = 3;
	float InteractingStartTime;
	float InteractingStopTime;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCapsuleComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cylinder;
	
	FTimerHandle InteractionTimerHandle;
	
	APawn* InteractingPawn;
};