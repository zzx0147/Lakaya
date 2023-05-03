#pragma once

#include "CoreMinimal.h"
#include "BattlePlayerController.h"
#include "InputActionValue.h"
#include "InteractablePlayerController.generated.h"

// DECLARE_EVENT_ThreeParams(AInteractablePlayerController, FInteractionStartSignature, const float&, AActor*, const float&);

// DECLARE_EVENT_OneParam(AInteractablePlayerController, FInteractionEndedSignature, const float&);

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
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
private:
	void StartInteraction(const FInputActionValue& Value);
	void StopInteraction(const FInputActionValue& Value);
	
public:
	/**
	 * @brief 긴 인터렉션이 시작되었을 때, 호출됩니다. 인터렉션 시작 시간, 대상 엑터를 매개변수로 받습니다.
	 * 이 이벤트는 오래 걸리는 상호작용시를 상정하여 선언되었으므로 즉시 실행되는 상호작용의 경우 호출되지 않을 수 있습니다.
	 */
	// FInteractionStartSignature OnInteractionStarted;

	/**
	 * @brief 긴 인터렉션이 중단되었을 때, 호출됩니다. 매개변수로 중단된 시간을 받습니다.
	 */
	// FInteractionEndedSignature OnInteractionStoppedNotify;
	
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
	// float InteractionRange;

	UPROPERTY(EditAnywhere, Category = Interaction)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	// UPROPERTY(Config)
	// float LockstepDelay = 0.1f;

	uint8 InteractableCount;
	TWeakObjectPtr<AActor> InteractingActor;
	FCollisionQueryParams TraceQueryParams;
	FTimerHandle InteractionTimer;
	FTimerHandle OwnerInteractionTimer;
	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSubSystem;
};