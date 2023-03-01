// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockstep.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS()
class LAKAYA_API AInteractableActor : public AActor
{
	GENERATED_BODY()

	DEFINE_LOCKSTEP_NOTIFY(InteractionStart, NetMulticast);
	DEFINE_LOCKSTEP_NOTIFY(InteractionStop, NetMulticast);

public:
	AInteractableActor();

protected:
	virtual void InteractionStart_Implementation(const float& Time, const FString& Arg) PURE_VIRTUAL(
		AInteractableActor::InteractionStart,);

	virtual bool InteractionStart_Validate(const float& Time, const FString& Arg) { return true; }

	virtual void InteractionStartNotify_Implementation(const float& Time, const FString& Arg) PURE_VIRTUAL(
		AInteractableActor::InteractionStartNotify_Implementation,)

	virtual void InteractionStop_Implementation(const float& Time, const FString& Arg) PURE_VIRTUAL(
		AInteractableActor::InteractionStop,);

	virtual bool InteractionStop_Validate(const float& Time, const FString& Arg) { return true; }

	virtual void InteractionStopNotify_Implementation(const float& Time, const FString& Arg) PURE_VIRTUAL(
		AInteractableActor::InteractionStopNotify_Implementation,)

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* InteractionTrigger;
};
