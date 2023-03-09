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
	CollisionChannel = ECC_GameTraceChannel3;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> InteractionContextFinder
	(TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_InteractionControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStartFinder
	(TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_InteractionStart'"));
	
	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStopFinder
	(TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_InteractionStop'"));
	
	if (InteractionContextFinder.Succeeded())
	{
		InteractionContext = InteractionContextFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("InteractionContextFinder is Succeeded"));		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionContextFinder is Null"));
	}
	
	if (InteractionStartFinder.Succeeded())
	{
		InteractionStartAction = InteractionStartFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("InteractionStartFinder is Succeeded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionStartFinder is Null"));
	}
	
	if (InteractionStopFinder.Succeeded())
	{
		InteractionStopAction = InteractionStopFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("InteractionStopFinder is Succeeded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionStopFinder is Null"));
	}
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

	if (!IsOwnedByLocalPlayer()) return;
	
	// Add interaction context when overlapped by trigger
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	++InteractableCount;
	if (!InputSystem->HasMappingContext(InteractionContext))
		InputSystem->AddMappingContext(InteractionContext, InteractionPriority);
}

void AInteractableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!IsOwnedByLocalPlayer()) return;
	
	// Remove interaction context when far away from triggers
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	--InteractableCount;
	if (InteractableCount == 0) InputSystem->RemoveMappingContext(InteractionContext);
}

void AInteractableCharacter::InteractionStart(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractionStart Function Start !"));
	FHitResult HitResult;
	const auto Location = GetCamera()->GetComponentLocation();
	const auto End = Location + GetCamera()->GetForwardVector() * InteractionRange;

	DrawDebugLine(GetWorld(), Location, End, FColor::Yellow, false, 2);
	
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, CollisionChannel, TraceQueryParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace failed to find an interactable object"));
		return;
	}

	InteractingActor = Cast<IInteractable>(HitResult.GetActor());
	//TODO: 상호작용중에 캐릭터가 이동할 수 없도록 해야 합니다.

	// if (InteractingActor.IsValid()) InteractingActor->Invoke(IInteractable::Execute_InteractionStart, this);
	if (!InteractingActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Interactable object does not implement IInteractable interface"));
		return;
	}

	InteractingActor->Invoke(IInteractable::Execute_InteractionStart, this);
}

void AInteractableCharacter::InteractionStop(const FInputActionValue& Value)
{
	if (InteractingActor.IsValid()) InteractingActor->Invoke(IInteractable::Execute_InteractionStop, this);
}