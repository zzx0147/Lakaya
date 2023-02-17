// Fill out your copyright notice in the Description page of Project Settings.


#include "PointCollectingPlayerState.h"

#include "Net/UnrealNetwork.h"

void APointCollectingPlayerState::GainPoint(const uint8& GainedPoint)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to gain a point. there is an logic error or client cheated"));
		return;
	}
	
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

void APointCollectingPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APointCollectingPlayerState, Point);
}
