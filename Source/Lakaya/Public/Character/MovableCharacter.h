// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonCharacter.h"
#include "MovableCharacter.generated.h"

UCLASS()
class LAKAYA_API AMovableCharacter : public AThirdPersonCharacter
{
	GENERATED_BODY()

public:
	AMovableCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Crouch(bool bClientSimulation = false) override;

	bool IsOwnedByLocalPlayer() const;
	void Run();
	void StopRun();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestSetRunState(bool IsRunning, const float& Time);

	UFUNCTION()
	void OnRep_IsRunning();

private:
	UPROPERTY(EditAnywhere, Category = Movement)
	float RunMultiplier;

	UPROPERTY(ReplicatedUsing=OnRep_IsRunning)
	bool bIsRunning;

	float RecentRunEventTime;
};
