// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageableCharacter.h"
#include "UObject/WeakInterfacePtr.h"
#include "InteractableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AInteractableCharacter : public ADamageableCharacter
{
	GENERATED_BODY()

public:
	AInteractableCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UFUNCTION(Server, Reliable)
	void RequestInteractionStart(const float& Time, AActor* Actor);

	UFUNCTION(Server, Reliable)
	void RequestInteractionStop(const float& Time, AActor* Actor);

private:
	void InteractionStart(const FInputActionValue& Value);
	void InteractionStop(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	UInputMappingContext* InteractionContext;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	int8 InteractionPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStartAction;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStopAction;

	UPROPERTY(EditAnywhere, Category = Interaction)
	float InteractionRange;

	UPROPERTY(EditAnywhere, Category = Interaction)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	uint8 InteractableCount;
	TWeakObjectPtr<AActor> InteractingActor;
	FCollisionQueryParams TraceQueryParams;
};
