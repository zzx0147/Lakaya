#include "PlayerController/InteractablePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/InteractableCharacter.h"

AInteractablePlayerController::AInteractablePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> InteractionContextFinder(
		TEXT("/Game/Dev/Yongwoo/Input/IC_InteractionControl"));
	
	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStartFinder(
		TEXT("/Game/Dev/Yongwoo/Input/IA_InteractionStart"));
	
	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStopFinder(
		TEXT("/Game/Dev/Yongwoo/Input/IA_InteractionStop"));

#pragma region Null 체크
	if (InteractionContextFinder.Succeeded()) InteractionContext = InteractionContextFinder.Object;
	else UE_LOG(LogTemp, Warning, TEXT("InteractionContextFinder is null."));
	if (InteractionStartFinder.Succeeded()) InteractionStartAction = InteractionStartFinder.Object;
	else UE_LOG(LogTemp, Warning, TEXT("InteractionStartFinder is null."));
	if (InteractionStopFinder.Succeeded()) InteractionStopAction = InteractionStopFinder.Object;
	else UE_LOG(LogTemp, Warning, TEXT("InteractionStopFinder is null."));
#pragma  endregion
}

void AInteractablePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(InteractionStartAction, ETriggerEvent::Triggered, this,
		&AInteractablePlayerController::StartInteraction);
	EnhancedInputComponent->BindAction(InteractionStopAction, ETriggerEvent::Triggered, this,
		&AInteractablePlayerController::StopInteraction);
}

void AInteractablePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);

	InputSubsystem->AddMappingContext(InteractionContext, InteractionPriority);
}

void AInteractablePlayerController::BeginPlay()
{
	Super::BeginPlay();
	TraceQueryParams.AddIgnoredActor(this);
}

void AInteractablePlayerController::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!InputSubSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;

	++InteractableCount;

	if (!InputSubSystem->HasMappingContext(InteractionContext))
	{
		InputSubSystem->AddMappingContext(InteractionContext, InteractionPriority);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, TEXT("Interaction context added"));
	}
}

void AInteractablePlayerController::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!InputSubSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;

	--InteractableCount;

	if (InteractableCount == 0)
	{
		InputSubSystem->RemoveMappingContext(InteractionContext);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, TEXT("Interaction context removed"));
	}
}

void AInteractablePlayerController::StartInteraction(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Client StartInteraction."));
	auto* InteractableCharacter = Cast<AInteractableCharacter>(GetCharacter());
	if (InteractableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractablePlayerController_InteractableCharacter is null."));
		return;
	}

	if (!InteractableCharacter->ShouldInteractStart()) return;
}

void AInteractablePlayerController::StopInteraction(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Client StopInteraction."));
	auto* InteractableCharacter = Cast<AInteractableCharacter>(GetCharacter());
	if (InteractableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractablePlayerController_InteractableCharacter is null."));
		return;
	}

	if (!InteractableCharacter->ShouldInteractStop()) return;	
}