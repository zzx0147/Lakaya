#include "Character/InteractableCharacter.h"

#include "Character/LakayaBasePlayerState.h"
#include "Interactable/Interactable.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/OccupationObject.h"

void AInteractableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInteractableCharacter, InteractingActor);
}

AInteractableCharacter::AInteractableCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bInteractionRequested = false;
}

void AInteractableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag(TEXT("Interactable")))
	{
		InteractableActor = OtherActor;
		// OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyActorBeginOverlap_InteractableActor is null."));
		return;
	}
}

void AInteractableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (OtherActor == InteractableActor)
	{
		InteractableActor = nullptr;
		// OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
}

bool AInteractableCharacter::ShouldInteractStart()
{
	if (InteractableActor.IsValid())
	{
		AOccupationObject* OccupationObject = Cast<AOccupationObject>(InteractableActor);
		if (OccupationObject == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShouldInteractStart_OccupationObject is null."));
			return false;
		}

		// if (const auto InteractablePlayerState = Cast<ALakayaBasePlayerState>(GetPlayerState()))
		// {
		// 	if (InteractablePlayerState->IsSameTeam(OccupationObject->GetObjectTeam()))
		// 	{
		// 		UE_LOG(LogTemp, Warning, TEXT("InteractableCharacter_이미 점령한 오브젝트입니다."));
		// 		return false;
		// 	}
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("InteractableCharacter_InteractablePlayerState is null."));
		// 	return false;
		// }

		
		
		if (OccupationObject->GetInteractingPawn() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShouldInteractStart_오브젝트를 누군가 상호작용하고 있습니다."));
			return false;
		}
		
		if (InteractionState != EInteractionState::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("InteractionState is not none."));
			return false;
		}

		bInteractionRequested = true;
		RequestInteractionStart(GetServerTime(), InteractableActor.Get());
		return true;
	}
	else
	{
		return false;
	}
}

bool AInteractableCharacter::ShouldInteractStop()
{
	if (InteractableActor.IsValid())
	{
		if (InteractionState != EInteractionState::OnGoing)
		{
			UE_LOG(LogTemp, Warning, TEXT("InteractionState is not ongoing."));
			// return false;
		}

		bInteractionRequested = false;
		RequestInteractionStop(GetServerTime(), InteractableActor.Get());
		return true;
	}
	else
	{
		return false;
	}
}

void AInteractableCharacter::InitializeInteraction()
{
	InteractingActor = nullptr;
	SetInteractionState(EInteractionState::None);
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}

void AInteractableCharacter::OnRep_InteractingActor()
{
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}

bool AInteractableCharacter::RequestInteractionStart_Validate(const float& Time, AActor* Actor)
{
	if (Actor && Actor->ActorHasTag("Interactable") && Time < GetServerTime() + 0.05f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("RequestInteractionStart_Validate completed."))
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestInteractionStart_Valide failed."));
		return false;
	}
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	InteractingActor = Actor;
	SetInteractionState(EInteractionState::OnGoing);
	// TODO :
	Cast<AInteractable>(Actor)->OnInteractionStart(Time, this);
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}

bool AInteractableCharacter::RequestInteractionStop_Validate(const float& Time, AActor* Actor)
{
	if (Actor && Actor->ActorHasTag("Interactable") && Time < GetServerTime() + 0.05f)
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestInteractionStop_Validate completed."));
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
	InitializeInteraction();
	Cast<AInteractable>(Actor)->OnInteractionStop(GetServerTime(), this);
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}
