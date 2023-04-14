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

private:
	virtual void OnInteractionStart(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(const float& Time, APawn* Caller) override;;
	virtual void OnCharacterDead(APawn* Caller) override;

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
};
