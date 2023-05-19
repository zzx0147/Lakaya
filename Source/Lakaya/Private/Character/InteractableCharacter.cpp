#include "Character/InteractableCharacter.h"

#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactable/Interactable.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/OccupationObject.h"

AInteractableCharacter::AInteractableCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bInteractionRequested = false;
}

void AInteractableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableCharacter, InteractionInfo);
}

void AInteractableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag(TEXT("Interactable")))
	{
		InteractableActor = OtherActor;
		OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyActorBeginOverlap_InteractableActor is null."));
	}
}

void AInteractableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (OtherActor == InteractableActor)
	{
		InteractableActor = nullptr;
		OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
}

bool AInteractableCharacter::ShouldInteract() const
{
	if (InteractableActor.IsValid())
	{
		const auto OccupationObject = Cast<AOccupationObject>(InteractableActor);
		if (OccupationObject == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShouldInteractStart_OccupationObject is null."));
			return false;
		}

		if (const auto InteractablePlayerState = Cast<ALakayaBasePlayerState>(GetPlayerState()))
		{
			if (InteractablePlayerState->GetTeam() == OccupationObject->GetObjectTeam())
			{
				UE_LOG(LogTemp, Warning, TEXT("InteractableCharacter_이미 점령한 오브젝트입니다."));
				return false;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InteractablePlayerState is null."));
			return false;
		}

		// 현재 오브젝트가 누군가 상호작용 중인지 체크합니다.
		if (OccupationObject->GetInteractingPawn() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShouldInteractStart_오브젝트를 누군가 상호작용하고 있습니다."));
			return false;
		}

		// 현재 상호작용 상태가 None인지 아닌지 체크합니다.
		if (InteractionInfo.InteractionState != EInteractionState::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("InteractionState is not none."));
			return false;
		}

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("상호작용이 가능한 오브젝트가 없습니다."));
		return false;
	}
}

void AInteractableCharacter::StartInteraction()
{
	if (!ShouldInteract()) return;

	bInteractionRequested = true;

	GetCharacterMovement()->DisableMovement();
	
	RequestInteractionStart(GetServerTime(), InteractableActor.Get());
}

void AInteractableCharacter::StopInteraction()
{
	if (!bInteractionRequested) return;

	bInteractionRequested = false;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	RequestInteractionStop(GetServerTime(), InteractableActor.Get());
}

void AInteractableCharacter::FinishInteraction(EInteractionState NewState, float Time)
{
	// 사용작용이 끝났을 때, 성공했다면 NewState = Success
	// 실패했다면, NewState = Stopped
	
	InteractionInfo.InteractionState = NewState;
	
	OnInteractionStateChanged.Broadcast(InteractionInfo);

	if (InteractionInfo.InteractionState == EInteractionState::None) return;
	
	Cast<AOccupationObject>(InteractionInfo.InteractingActor)->OnInteractionFinish(this);

	InteractionInfo.InteractingActor = nullptr;
	
	if (InteractionInfo.InteractionState == EInteractionState::Success)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			SetInteractionState(EInteractionState::None);
		});
		GetWorldTimerManager().SetTimer(InteractionClearTimer, TimerDelegate, 1.0f, false);
	}
}

void AInteractableCharacter::OnRep_InteractingActor() const
{
	OnInteractingActorChanged.Broadcast(InteractionInfo.InteractingActor.Get());
}

bool AInteractableCharacter::RequestInteractionStart_Validate(const float& Time, AActor* Actor)
{
	if (!ShouldInteract()) return false;
	
	if (Actor && Actor->ActorHasTag("Interactable") && Time < GetServerTime() + 0.05f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	InteractionInfo.InteractingActor = Actor;
	SetInteractionState(EInteractionState::OnGoing);
	OnInteractionStateChanged.Broadcast(InteractionInfo);
	OnRep_InteractingActor();
	Cast<AInteractable>(Actor)->OnInteractionStart(Time, this);
}

bool AInteractableCharacter::RequestInteractionStop_Validate(const float& Time, AActor* Actor)
{
	if (Actor && Actor->ActorHasTag("Interactable") && Time < GetServerTime() + 0.05f)
	{
		if (InteractionInfo.InteractingActor == nullptr ||
			InteractionInfo.InteractionState != EInteractionState::OnGoing)
		{
			return false;
		}
		
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestInteractionStop_Validate failed."));
		return false;
	}
}

void AInteractableCharacter::RequestInteractionStop_Implementation(const float& Time, AActor* Actor)
{
	// InteractingActor = nullptr;
	// InteractionInfo.InteractingActor = nullptr;
	// SetInteractionState(EInteractionState::None);
	// OnInteractingActorChanged.Broadcast(InteractingActor.Get());
	// OnInteractingActorChanged.Broadcast(InteractionInfo.InteractingActor.Get());
	OnRep_InteractingActor();
	Cast<AInteractable>(Actor)->OnInteractionStop(GetServerTime(), this);
}