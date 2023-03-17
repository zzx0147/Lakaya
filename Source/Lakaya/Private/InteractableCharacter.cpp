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
	// TODO : 주석 해제
	// if (!InputSystem->HasMappingContext(InteractionContext))
	// InputSystem->AddMappingContext(InteractionContext, InteractionPriority);
	if (auto PlayerController = Cast<APlayerController>(Controller))
		if (auto LocalPlayer = PlayerController->GetLocalPlayer())
		{
			InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (InputSystem.IsValid()) InputSystem->AddMappingContext(InteractionContext, InteractionPriority);
		}
}

void AInteractableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!IsOwnedByLocalPlayer()) return;

	// Add interaction context when overlapped by trigger
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	++InteractableCount;
	// TODO : 주석 해제
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
	// TODO : 주석해제
	// if (InteractableCount == 0) InputSystem->RemoveMappingContext(InteractionContext);
}

void AInteractableCharacter::OnInteractionSuccess_Implementation()
{
	SimulatedFocusSpace[EFocusKey::MainHand] = false;
	InteractingActor = nullptr;
}

void AInteractableCharacter::OnInteractionCanceled_Implementation()
{
	SimulatedFocusSpace[EFocusKey::MainHand] = false;
	InteractingActor = nullptr;
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	if (IsFocussed(EFocusKey::MainHand)) OnInteractionCanceled();
	else if (auto Interactable = Cast<IInteractable>(Actor))
	{
		FocusSpace.FindOrAdd(EFocusKey::MainHand) = true;
		Interactable->InteractionStart(Time, this);
	}
	else UE_LOG(LogActor, Warning, TEXT("Requested interaction actor was not IInteractable"));
}

void AInteractableCharacter::RequestInteractionStop_Implementation(const float& Time, AActor* Actor)
{
	if (auto Interactable = Cast<IInteractable>(Actor))
	{
		FocusSpace[EFocusKey::MainHand] = false;
		Interactable->InteractionStop(Time, this);
	}
	else UE_LOG(LogActor, Warning, TEXT("Requested interaction actor was not IInteractable"));
}

void AInteractableCharacter::InteractionStart(const FInputActionValue& Value)
{
	if (IsFocussed(EFocusKey::MainHand,true)) return;
	FHitResult HitResult;
	const auto Location = GetCamera()->GetComponentLocation();
	const auto End = Location + GetCamera()->GetForwardVector() * InteractionRange;

	DrawDebugLine(GetWorld(), Location, End, FColor::Yellow, false, 2);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, CollisionChannel, TraceQueryParams))
		return;
	
	if (auto Actor = HitResult.GetActor())
		if (Actor->Implements<UInteractable>())
		{
			SimulatedFocusSpace.FindOrAdd(EFocusKey::MainHand) = true;
			RequestInteractionStart(GetServerTime(), Actor);
			InteractingActor = Actor;
		}
}

void AInteractableCharacter::InteractionStop(const FInputActionValue& Value)
{
	if (!InteractingActor.IsValid()) return;
	SimulatedFocusSpace[EFocusKey::MainHand] = false;
	RequestInteractionStop(GetServerTime(), InteractingActor.Get());
	InteractingActor = nullptr;
}