// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/InteractableCharacter.h"

#include "Interactable/Interactable.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/OccupationObject.h"

void AInteractableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInteractableCharacter, InteractingActor);
}

void AInteractableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor->Implements<UInteractable>())
	{
		InteractableActor = OtherActor;
		OnInteractableActorChanged.Broadcast(InteractableActor.Get());
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
		OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
}

void AInteractableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacter(EventInstigator, DamageCauser);
	if (InteractingActor.IsValid()) Cast<IInteractable>(InteractingActor)->OnCharacterDead(this);
}

bool AInteractableCharacter::ShouldInteract()
{
	if (InteractableActor.IsValid())
	{
		AOccupationObject* OccupationObject = Cast<AOccupationObject>(InteractableActor);
		if (OccupationObject == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShouldInteract_OccupationObject is null."));
			return false;
		}
		if (OccupationObject->GetInteractingPawn() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShouldInteract_GetInteractingPawn is valid."));
			return false;
		}

		RequestInteractionStart(GetServerTime(), InteractableActor.Get());
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShouldInteract_InValid is null."));
		return false;
	}
}

void AInteractableCharacter::InitializeInteraction()
{
	InteractingActor = nullptr;
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}

void AInteractableCharacter::OnRep_InteractingActor()
{
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}

bool AInteractableCharacter::RequestInteractionStart_Validate(const float& Time, AActor* Actor)
{
	return Actor && Actor->Implements<UInteractable>() && Time < GetServerTime() + 0.05f;
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	InteractingActor = Actor;
	Cast<IInteractable>(Actor)->OnInteractionStart(Time, this);
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}

bool AInteractableCharacter::RequestInteractionStop_Validate(const float& Time, AActor* Actor)
{
	return Actor && Actor->Implements<UInteractable>() && Time < GetServerTime() + 0.05f;
}

void AInteractableCharacter::RequestInteractionStop_Implementation(const float& Time, AActor* Actor)
{
	InteractingActor = nullptr;
	Cast<IInteractable>(Actor)->OnInteractionStop(GetServerTime(), this);
	OnInteractingActorChanged.Broadcast(InteractingActor.Get());
}