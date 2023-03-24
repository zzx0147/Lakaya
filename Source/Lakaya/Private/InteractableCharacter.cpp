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
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow,TEXT("Interaction context added"));
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
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow,TEXT("Interaction context removed"));
	}
}

void AInteractableCharacter::InitiateInteractionStart(const float& Time, AActor* Actor, const float& Duration)
{
	// Execute InteractionStartNotify when Duration is longer then LockstepDelay
	if (Duration > LockstepDelay)
	{
		InteractionStartNotify(Time, Actor, Duration);
		return;
	}

	InitiateLockstepEvent(Time, [this,Actor]
	{
		// Execute OnInteractionStart when It's focusable
		if (IsFocussedBy(EFocusContext::Server, EFocusSpace::MainHand))
			Cast<IInteractable>(Actor)->OnInteractionStart(this);
		else UE_LOG(LogActor, Error, TEXT("FocusState was not None on InitiateInteractionStart! FocusState was %d"),
		            GetFocusState(EFocusContext::Server,EFocusSpace::MainHand));
	});
}

void AInteractableCharacter::NoticeInstantInteractionLocal()
{
	if (ReleaseFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
		InteractingActor = nullptr;
	else UE_LOG(LogActor, Error, TEXT("Fail to release focus on NoticeInstantInteractionLocal! FocusState was %d"),
	            GetFocusState(EFocusContext::Owner,EFocusSpace::MainHand));
}

void AInteractableCharacter::InteractionStopNotify_Implementation(const float& Time, AActor* Actor)
{
	InitiateLockstepEvent(Time, [this,Time,Actor]
	{
		if (HasAuthority())
		{
			if (ReleaseFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Interacting))
				Cast<IInteractable>(Actor)->OnInteractionStop(this);
			else UE_LOG(LogActor, Error, TEXT("Fail to release focus on InteractionStopNotify with authority!"));
		}

		if (ReleaseFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Interacting))
			OnInteractionStoppedNotify.Broadcast(Time);
		else UE_LOG(LogActor, Error, TEXT("Fail to release focus on InteractionStopNotify!"));
	});
}

void AInteractableCharacter::InteractionStartNotify_Implementation(const float& Time, AActor* Actor,
                                                                   const float& Duration)
{
	InitiateLockstepEvent(Time, [this,Time,Actor,Duration]
	{
		// Execute OnInteractionStart when It's server and focusable.
		if (HasAuthority())
		{
			if (SetFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Interacting))
				Cast<IInteractable>(Actor)->OnInteractionStart(this);
			else UE_LOG(LogActor, Error,
			            TEXT("Fail to set focus on InteractionStartNotify with authority! FocusState was %d"),
			            GetFocusState(EFocusContext::Server,EFocusSpace::MainHand));
		}

		if (SetFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Interacting))
		{
			if (IsOwnedByLocalPlayer())
			{
				GetWorldTimerManager().SetTimer(OwnerInteractionTimer, [this]
				{
					if (ReleaseFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
					{
						InteractingActor = nullptr;
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,
						                                 TEXT("Interaction Completed in Owner!"));
					}
					else
						UE_LOG(LogActor, Error,
					       TEXT("Fail to release focus on InteractionStartNotify owner context! FocusState was %d"),
					       GetFocusState(EFocusContext::Owner,EFocusSpace::MainHand));
				}, Duration - LockstepDelay, false);
			}

			GetWorldTimerManager().SetTimer(InteractionTimer, [this]
			{
				if (ReleaseFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Interacting))
					GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green,
					                                 TEXT("Interaction Completed in Simulated!"));
				else UE_LOG(LogActor, Error, TEXT("Fail to release focus on InteractionStartNotify! FocusState was %d"),
				            GetFocusState(EFocusContext::Simulated,EFocusSpace::MainHand));
			}, Duration, false);
			
			OnInteractionStarted.Broadcast(Time, Actor, Duration);
		}
		else UE_LOG(LogActor, Error, TEXT("Fail to set focus on InteractionStartNotify! FocusState was %d"),
		            GetFocusState(EFocusContext::Simulated,EFocusSpace::MainHand));
	});
}

bool AInteractableCharacter::RequestInteractionStart_Validate(const float& Time, AActor* Actor)
{
	return Actor && Actor->Implements<UInteractable>();
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	Cast<IInteractable>(Actor)->OnServerInteractionBegin(GetServerTime(), this);
}

bool AInteractableCharacter::RequestInteractionStop_Validate(const float& Time, AActor* Actor)
{
	return Actor && Actor->Implements<UInteractable>();
}

void AInteractableCharacter::RequestInteractionStop_Implementation(const float& Time, AActor* Actor)
{
	if (IsFocussedBy(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Interacting))
		Cast<IInteractable>(Actor)->OnServerInteractionStopBegin(GetServerTime(), this);
	else UE_LOG(LogActor, Error, TEXT("InteractionStop was requested but It was not focussed by Interacting!"));
}

void AInteractableCharacter::InitiateLockstepEvent(const float& Time, std::function<void()> Callback)
{
	auto TimerDelay = Time + LockstepDelay - GetServerTime();
	if (TimerDelay > 0)
	{
		TSharedPtr<FTimerHandle> Timer = MakeShared<FTimerHandle>();
		GetWorldTimerManager().SetTimer(*Timer, [Callback,Ptr = Timer] { Callback(); }, TimerDelay, false);
	}
	else if (TimerDelay == 0) Callback();
	else UE_LOG(LogActor, Warning, TEXT("InititateLockstepEvent::TimerDelay was negative number! (%f)"), TimerDelay);
}

void AInteractableCharacter::InteractionStart(const FInputActionValue& Value)
{
	FHitResult HitResult;
	const auto Location = GetCamera()->GetComponentLocation();
	const auto End = Location + GetCamera()->GetForwardVector() * InteractionRange;

	DrawDebugLine(GetWorld(), Location, End, FColor::Yellow, false, 2);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, CollisionChannel, TraceQueryParams))
		if (auto Actor = HitResult.GetActor())
			if (Actor->Implements<UInteractable>()
				&& SetFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
			{
				//When hit actor was implement IInteractable and this character was Focusable
				InteractingActor = Actor;
				Cast<IInteractable>(Actor)->OnLocalInteractionBegin(this);
				RequestInteractionStart(GetServerTime(), Actor);
			}
}

void AInteractableCharacter::InteractionStop(const FInputActionValue& Value)
{
	if (!InteractingActor.IsValid()) return;
	if (ReleaseFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Interacting))
	{
		Cast<IInteractable>(InteractingActor.Get())->OnLocalInteractionStopBegin(this);
		RequestInteractionStop(GetServerTime(), InteractingActor.Get());
		InteractingActor = nullptr;
	}
	else UE_LOG(LogActor, Error, TEXT("It was posssible to InteractionStop but It was not focussed by Interacting!"))
}
