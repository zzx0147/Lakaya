// Fill out your copyright notice in the Description page of Project Settings.


#include "PointCollectingPlayerState.h"

#include "Net/UnrealNetwork.h"

void APointCollectingPlayerState::GainPoint(const uint8& GainedPoint)
{
	Point += GainedPoint;
}

void APointCollectingPlayerState::ResetPoint()
{
	Point = 0;
}

const uint8& APointCollectingPlayerState::GetPoint() const
{
	return Point;
}

void APointCollectingPlayerState::BeginPlay()
{
	Super::BeginPlay();

	Point = 0;
}

void APointCollectingPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APointCollectingPlayerState, Point);
}
