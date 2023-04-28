// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaBasePlayerState.h"
#include "StatPlayerState.generated.h"

UCLASS()
class LAKAYA_API AStatPlayerState : public ALakayaBasePlayerState
{
	GENERATED_BODY()

public:
	virtual float GetMaxHealth() const override;
};
