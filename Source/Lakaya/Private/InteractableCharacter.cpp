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
	InteractionRange = 500;
	CollisionChannel = ECC_GameTraceChannel3;

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
		InputSystem->AddMappingContext(InteractionContext, InteractionPriority);
}

void AInteractableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	// Remove interaction context when far away from triggers
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	--InteractableCount;
	if (InteractableCount == 0) InputSystem->RemoveMappingContext(InteractionContext);
}

void AInteractableCharacter::InteractionStart(const FInputActionValue& Value)
{
	FHitResult HitResult;
	const auto Location = GetCamera()->GetComponentLocation();
	const auto End = Location + GetCamera()->GetForwardVector() * InteractionRange;

	DrawDebugLine(GetWorld(), Location, End, FColor::Yellow, false, 2, 0, 1);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, CollisionChannel, TraceQueryParams))
		return;

	InteractingActor = Cast<IInteractable>(HitResult.GetActor());
	if (InteractingActor.IsValid()) InteractingActor->Execute(IInteractable::Execute_InteractionStart, this);
}

void AInteractableCharacter::InteractionStop(const FInputActionValue& Value)
{
	if (InteractingActor.IsValid()) InteractingActor->Execute(IInteractable::Execute_InteractionStop, this);
}
