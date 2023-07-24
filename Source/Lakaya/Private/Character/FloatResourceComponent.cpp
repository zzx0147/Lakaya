// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FloatResourceComponent.h"

#include "Net/UnrealNetwork.h"

UFloatResourceComponent::UFloatResourceComponent()
{
	DefaultMaxResource = DefaultResource = 100.f;
}

void UFloatResourceComponent::CostResource(const uint64& Value)
{
	CostResource(static_cast<double>(Value));
}

void UFloatResourceComponent::CostResource(const double& Value)
{
	const auto FixedValue = FMath::Abs(Value);
	Resource = bAllowNegative || IsEnough(FixedValue) ? Resource - FixedValue : 0.f;
	OnRep_Resource();
}

void UFloatResourceComponent::GainResource(const uint64& Value)
{
	GainResource(static_cast<double>(Value));
}

void UFloatResourceComponent::GainResource(const double& Value)
{
	Resource = FMath::Max(GetMaxResource(), Resource + FMath::Abs(Value));
}

bool UFloatResourceComponent::IsEnough(const uint64& Value) const
{
	return IsEnough(static_cast<double>(Value));
}

bool UFloatResourceComponent::IsEnough(const double& Value) const
{
	return Resource >= Value;
}

bool UFloatResourceComponent::IsFull() const
{
	const auto Max = GetMaxResource();
	return FMath::IsNearlyEqual(Resource, Max) || Resource > Max;
}

void UFloatResourceComponent::Clear()
{
}

void UFloatResourceComponent::Reset()
{
}

double UFloatResourceComponent::GetMaxResource() const
{
	//TODO: 스탯 연동 필요
	return DefaultMaxResource;
}

void UFloatResourceComponent::OnRep_Resource()
{
	OnResourceUpdated.Broadcast(Resource);
}

void UFloatResourceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFloatResourceComponent, Resource);
}
