// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "ThirdPersonCharacter.h"
#include "MovableCharacter.generated.h"

UCLASS()
class LAKAYA_API AMovableCharacter : public AThirdPersonCharacter
{
	GENERATED_BODY()

public:
	AMovableCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	bool IsOwnedByLocalPlayer() const;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

private:
	// Input event functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Crouching(const FInputActionValue& Value);
	void UnCrouching(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRunning(const FInputActionValue& Value);

protected:
	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSystem;

private:
	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	class UInputMappingContext* MovementContext;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	int8 MovementContextPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* UnCrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* StopRunningAction;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunMultiplier;
};
