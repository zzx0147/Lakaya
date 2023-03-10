// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectorPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ACollectorPlayerState::GainPoint(const uint8& GainedPoint)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to gain a point. there is an logic error or client cheated"));
		return;
	}

	Point += GainedPoint;

	// 점수가 변경 됐을 경우, 다른 클라이언트들에게 동기화
	OnRep_Score();
}

void ACollectorPlayerState::ResetPoint()
{
	Point = 0;
}

const uint8& ACollectorPlayerState::GetPoint() const
{
	return Point;
}

void ACollectorPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectorPlayerState, Point);
}
