// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "MenuCallingPlayerController.h"
#include "MovablePlayerController.generated.h"

UCLASS()
class LAKAYA_API AMovablePlayerController : public AMenuCallingPlayerController
{
	GENERATED_BODY()

public:
	AMovablePlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	virtual void SetupMappingContext(class UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem);
	virtual void OnPossessedPawnChangedCallback(APawn* OldPawn, APawn* NewPawn);

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Crouch(const FInputActionValue& Value);
	void UnCrouch(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

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
	UInputAction* StopRunAction;

	TWeakObjectPtr<class AMovableCharacter> MovableCharacter;
};
