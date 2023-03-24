// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugInstantInteractable.h"
#include "GameFramework/Actor.h"
#include "DebugLongInteractable.generated.h"

UCLASS()
class LAKAYA_API ADebugLongInteractable : public ADebugInstantInteractable
{
	GENERATED_BODY()

public:
	virtual void OnLocalInteractionBegin(APawn* Caller) override { return; }
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;
	virtual void OnLocalInteractionStopBegin(APawn* Caller) override;
	virtual void OnServerInteractionStopBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(APawn* Caller) override;
};
