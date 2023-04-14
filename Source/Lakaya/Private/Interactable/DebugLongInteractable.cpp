// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/DebugLongInteractable.h"

#include "Character/InteractableCharacter.h"


// Sets default values
// void ADebugLongInteractable::OnServerInteractionBegin(const float& Time, APawn* Caller)
// {
// 	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
// 		CastedCaller->InitiateInteractionStart(Time, this, 5.f);
// 	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionBegin::Caller was not AInteractableCharacter!"));
// }
//
// void ADebugLongInteractable::OnInteractionStart(APawn* Caller)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Long Interaction Start!"));
// }
//
// void ADebugLongInteractable::OnLocalInteractionStopBegin(APawn* Caller)
// {
// }
//
// void ADebugLongInteractable::OnServerInteractionStopBegin(const float& Time, APawn* Caller)
// {
// 	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
// 		CastedCaller->InteractionStopNotify(Time, this);
// 	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionStopBegin::Caller was not AInteractableCharacter!"));
// }
//
// void ADebugLongInteractable::OnInteractionStop(APawn* Caller)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Long Interaction Stop!"));
// }
void ADebugLongInteractable::OnInteractionStart(const float& Time, APawn* Caller)
{
}

void ADebugLongInteractable::OnInteractionStop(const float& Time, APawn* Caller)
{
}

void ADebugLongInteractable::OnCharacterDead(APawn* Caller)
{
}
