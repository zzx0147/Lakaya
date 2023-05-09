// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/StartRemoteCallAbility.h"

#include "GameFramework/GameStateBase.h"


void UStartRemoteCallAbility::AbilityStart()
{
	Super::AbilityStart();
	RequestStart(GetServerTime());
}

bool UStartRemoteCallAbility::RequestStart_Validate(const float& RequestTime)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return GetServerTime() + 0.05f >= RequestTime;
}

float UStartRemoteCallAbility::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}
