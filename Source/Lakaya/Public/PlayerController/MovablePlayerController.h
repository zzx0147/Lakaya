// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLobbyPlayerController.h"
#include "MovablePlayerController.generated.h"

UCLASS()
class LAKAYA_API AMovablePlayerController : public AGameLobbyPlayerController
{
	GENERATED_BODY()

public:
	AMovablePlayerController();

protected:
	virtual void SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent) override;
	virtual void SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem) override;
	virtual void OnPossessedPawnChangedCallback(APawn* OldPawn, APawn* NewPawn) override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Crouch(const FInputActionValue& Value);
	void UnCrouch(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	UInputMappingContext* MovementContext;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	int8 MovementContextPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* MoveAction;

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
