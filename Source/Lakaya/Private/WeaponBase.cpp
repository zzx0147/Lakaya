// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "GameFramework/GameStateBase.h"

// Add default functionality here for any IWeaponBase functions that are not pure virtual.
void IWeaponBase::Invoke(void (* Func)(UObject*, const float&))
{
	const auto Object = Cast<UObject>(this);
	Func(Object, Object->GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}
