// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputMappingQuery.h"
#include "GameFramework/Character.h"
#include "BasePlayerCharacter.generated.h"

UCLASS()
class LAKAYA_API ABasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* BasicControlContext;

	UPROPERTY(EditAnywhere, Category = Input)
	int8 ContextPriority;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* UnCrouchAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* StopRunningAction;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunMultiplier;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* Camera;
public:
	// Sets default values for this character's properties
	ABasePlayerCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Crouching(const FInputActionValue& Value);
	void UnCrouching(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRunning(const FInputActionValue& Value);
};
