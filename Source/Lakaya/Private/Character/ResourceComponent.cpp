// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ResourceComponent.h"


UResourceComponent::UResourceComponent()
{
	SetIsReplicatedByDefault(true);
}

bool UResourceComponent::TryCostResource(const uint64& Value)
{
	const auto Result = IsEnough(Value);
	if (Result) CostResource(Value);
	return Result;
}

bool UResourceComponent::TryCostResource(const double& Value)
{
	const auto Result = IsEnough(Value);
	if (Result) CostResource(Value);
	return Result;
}

void UResourceComponent::OnAliveStateChanged(const bool& AliveState)
{
	// 살아나면 자원이 다시 채워지도록 합니다.
	if (AliveState && GetOwner()->HasAuthority()) Reset();
}
