// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "DebugInteractable.generated.h"

UCLASS()
class LAKAYA_API ADebugInteractable : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADebugInteractable();
	virtual void InteractionStart(const float& Time, APawn* Caller) override;
	virtual void InteractionStop(const float& Time, APawn* Caller) override;

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
};
