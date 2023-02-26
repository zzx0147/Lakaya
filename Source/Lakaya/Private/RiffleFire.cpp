// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFire.h"

#include "GameFramework/GameStateBase.h"

void URiffleFire::SwitchFireMode()
{
}

void URiffleFire::FireStart_Implementation(const float& Time)
{
}

void URiffleFire::FireStop_Implementation(const float& Time)
{
}

void URiffleFire::TraceFire()
{
	GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}
