// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/CharacterAbility.h"

#include "GameFramework/GameStateBase.h"

void UCharacterAbility::AbilityStart()
{
}

void UCharacterAbility::AbilityStop()
{
}

void UCharacterAbility::RequestStop_Implementation(const float& RequestTime)
{
}

bool UCharacterAbility::RequestStop_Validate(const float& RequestTime)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds() + 0.05f >= RequestTime;
}

void UCharacterAbility::RequestStart_Implementation(const float& RequestTime)
{
}

bool UCharacterAbility::RequestStart_Validate(const float& RequestTime)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds() + 0.05f >= RequestTime;
}
