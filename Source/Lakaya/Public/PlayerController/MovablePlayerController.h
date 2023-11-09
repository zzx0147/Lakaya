// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaPlayerController.h"
#include "MovablePlayerController.generated.h"

struct FInputActionValue;

UCLASS()
class LAKAYA_API AMovablePlayerController : public ALakayaPlayerController
{
	GENERATED_BODY()

public:
	AMovablePlayerController();

	void SetMouseSensitivity(float Sensitivity) { MouseSensitivity = Sensitivity; }
	float GetMouseSensitivity() {return MouseSensitivity; }

protected:
	virtual void BeginPlay() override;

	virtual void SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent) override;
	virtual void SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem) override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	UInputMappingContext* MovementContext;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	int8 MovementContextPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Actions")
	UInputAction* LookAction;

	float MouseSensitivity;
};
