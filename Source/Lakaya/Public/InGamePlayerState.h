// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "InGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AInGamePlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	float Health;

public:
	void ApplyDamage(const float&);
	const float& GetHealth() const;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
