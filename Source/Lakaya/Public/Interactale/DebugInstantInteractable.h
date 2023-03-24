// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "DebugInstantInteractable.generated.h"

UCLASS()
class LAKAYA_API ADebugInstantInteractable : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADebugInstantInteractable();
	virtual void OnLocalInteractionBegin(APawn* Caller) override;
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
};
