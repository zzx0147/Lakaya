// Fill out your copyright notice in the Description page of Project Settings.


#include "Lockstep.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ILockstep::Execute(void (* ExecuteFunction)(UObject*, const float&))
{
	const auto Object = Cast<UObject>(this);
	ExecuteFunction(Object, Object->GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}

void ILockstep::Execute(void (*ExecuteFunction)(UObject*, const float&, const FUniqueNetIdRepl&), const APawn* Caller)
{
	const auto Object = Cast<UObject>(this);
	ExecuteFunction(Object, Object->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(),
	                Caller->GetPlayerState()->GetUniqueId());
}
