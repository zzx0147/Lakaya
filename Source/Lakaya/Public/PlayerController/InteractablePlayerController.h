#pragma once

#include "CoreMinimal.h"
#include "BattlePlayerController.h"
#include "InputActionValue.h"
#include "InteractablePlayerController.generated.h"

UCLASS()
class LAKAYA_API AInteractablePlayerController : public ABattlePlayerController
{
	GENERATED_BODY()
	
public:	
	AInteractablePlayerController();

	virtual void SetupEnhancedInputComponent(class UEnhancedInputComponent* const& EnhancedInputComponent) override;
	virtual void SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem) override;

protected:
	virtual void BeginPlay() override;
	// virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	// virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
private:
	void StartInteraction(const FInputActionValue& Value);
	void StopInteraction(const FInputActionValue& Value);
	
private:
	UPROPERTY(EditAnywhere, Category = "IC_Interaction")
	UInputMappingContext* InteractionContext;

	UPROPERTY(EditAnywhere, Category = "IA_InteractionStart")
	UInputAction* InteractionStartAction;

	UPROPERTY(EditAnywhere, Category = "IA_InteractionStop")
	UInputAction* InteractionStopAction;

	UPROPERTY(EditAnywhere)
	uint8 InteractionPriority;
	
	// UPROPERTY(EditAnywhere, Category = Interaction)
	// TEnumAsByte<ECollisionChannel> CollisionChannel;

	// uint8 InteractableCount;
	// TWeakObjectPtr<AActor> InteractingActor;
	// FCollisionQueryParams TraceQueryParams;
	// FTimerHandle InteractionTimer;
	// TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSubSystem;
};