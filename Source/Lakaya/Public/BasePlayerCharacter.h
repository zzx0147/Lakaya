// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "BasePlayerCharacter.generated.h"

UCLASS()
class LAKAYA_API ABasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	class UInputMappingContext* BasicControlContext;

	UPROPERTY(EditAnywhere, Category = "Input|Movement|Context")
	int8 BasicContextPriority;

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

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	UInputMappingContext* InteractionContext;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	int8 InteractionPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStartAction;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStopAction;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunMultiplier;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = Interaction)
	float InteractionRange;

public:
	// Sets default values for this character's properties
	ABasePlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSystem;

private:
	// Input event functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Crouching(const FInputActionValue& Value);
	void UnCrouching(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRunning(const FInputActionValue& Value);
	void InteractionStart(const FInputActionValue& Value);
	void InteractionStop(const FInputActionValue& Value);

	uint8 InteractableCount;
	TWeakObjectPtr<AActor> InteractingActor;
};
