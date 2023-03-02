// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.
void IInteractable::Invoke(void (* Func)(UObject*, const float&, const FString&), APawn* Caller)
{
	const auto Object = Cast<UObject>(this);
	Func(Object, Object->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(),
	     Caller->GetPlayerState()->GetUniqueId().ToString());
}
