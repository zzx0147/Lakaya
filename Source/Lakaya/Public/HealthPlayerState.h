// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HealthPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AHealthPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void SetupPlayerState(APawn* Pawn);
	const float& GetHealth() const;

private:
	UFUNCTION()
	void ApplyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
	                 AActor* DamageCauser);

	// Client must NOT change this value
	UPROPERTY(Replicated, Transient)
	float Health;
};
