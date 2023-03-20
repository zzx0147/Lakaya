// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "Interactable.h"
#include "GameFramework/GameStateBase.h"

AInteractableCharacter::AInteractableCharacter()
{
	if (IsRunningDedicatedServer()) return;

	InteractionRange = 500;
	CollisionChannel = ECC_Camera;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> InteractionContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_InteractionControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_InteractionStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_InteractionStop'"));

	if (InteractionContextFinder.Succeeded()) InteractionContext = InteractionContextFinder.Object;
	if (InteractionStartFinder.Succeeded()) InteractionStartAction = InteractionStartFinder.Object;
	if (InteractionStopFinder.Succeeded()) InteractionStopAction = InteractionStopFinder.Object;
}

void AInteractableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto CastedComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		CastedComponent->BindAction(InteractionStartAction, ETriggerEvent::Triggered, this,
		                            &AInteractableCharacter::InteractionStart);
		CastedComponent->BindAction(InteractionStopAction, ETriggerEvent::Triggered, this,
		                            &AInteractableCharacter::InteractionStop);
	}
}

void AInteractableCharacter::BeginPlay()
{
	Super::BeginPlay();
	TraceQueryParams.AddIgnoredActor(this);
}

void AInteractableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Add interaction context when overlapped by trigger
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	++InteractableCount;
	if (!InputSystem->HasMappingContext(InteractionContext))
	{
		InputSystem->AddMappingContext(InteractionContext, InteractionPriority);
		GEngine->AddOnScreenDebugMessage(-1,3,FColor::Yellow,TEXT("Interaction context added"));
	}
}

void AInteractableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	// Remove interaction context when far away from triggers
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	--InteractableCount;
	if (InteractableCount == 0)
	{
		InputSystem->RemoveMappingContext(InteractionContext);
		GEngine->AddOnScreenDebugMessage(-1,3,FColor::Yellow,TEXT("Interaction context removed"));
	}
}

bool AInteractableCharacter::RequestInteractionStart_Validate(const float& Time, AActor* Actor)
{
	return Actor && Actor->Implements<UInteractable>()
		&& SetFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Interacting);
}

void AInteractableCharacter::InteractionEndedNotify_Implementation(const float& Time, bool IsSuccess)
{
	if (HasAuthority() && !ReleaseFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Interacting))
		UE_LOG(LogActor, Error, TEXT("Focus release fail on InteractionEnding with authority!"));

	if (IsOwnedByLocalPlayer())
	{
		if (!ReleaseFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
			UE_LOG(LogActor, Error, TEXT("Focus release fail on InteractionEnding by local player"));

		InteractingActor = nullptr;
	}

	ReleaseFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Interacting);
	OnInteractionEnded.Broadcast(Time, IsSuccess);
}

void AInteractableCharacter::InteractionStartNotify_Implementation(const float& Time, AActor* Actor)
{
	if (!SetFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Interacting))
		UE_LOG(LogActor, Error, TEXT("Focus release fail on InteractionStartNotify with Simulated"));

	OnInteractionStarted.Broadcast(Time, Actor);
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	Cast<IInteractable>(Actor)->InteractionStart(GetServerTime(), this);
}

void AInteractableCharacter::RequestInteractionStop_Implementation(const float& Time, AActor* Actor)
{
	Cast<IInteractable>(Actor)->InteractionStop(GetServerTime(), this);
}

bool AInteractableCharacter::RequestInteractionStop_Validate(const float& Time, AActor* Actor)
{
	return Actor && Actor->Implements<UInteractable>()
		&& ReleaseFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Interacting);
}

void AInteractableCharacter::InteractionStart(const FInputActionValue& Value)
{
	FHitResult HitResult;
	const auto Location = GetCamera()->GetComponentLocation();
	const auto End = Location + GetCamera()->GetForwardVector() * InteractionRange;

	DrawDebugLine(GetWorld(), Location, End, FColor::Yellow, false, 2);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, CollisionChannel, TraceQueryParams))
		return;

	if (auto Actor = HitResult.GetActor())
		if (Actor->Implements<UInteractable>()
			&& SetFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
		{
			RequestInteractionStart(GetServerTime(), Actor);
			InteractingActor = Actor;
		}
}

void AInteractableCharacter::InteractionStop(const FInputActionValue& Value)
{
	if (!InteractingActor.IsValid()) return;
	if (!ReleaseFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
		UE_LOG(LogActor, Error, TEXT("Focus release fail on InteractionStop!"));
	RequestInteractionStop(GetServerTime(), InteractingActor.Get());
	InteractingActor = nullptr;
}
