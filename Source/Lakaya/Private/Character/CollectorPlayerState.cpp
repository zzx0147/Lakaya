// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CollectorPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACollectorPlayerState::ACollectorPlayerState()
{
	Point = 0;
	Energy = 0;
	Money = 0;
}

void ACollectorPlayerState::GainPoint(const uint8& GainedPoint)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to gain a point. there is an logic error or client cheated"));
		return;
	}

	Point += GainedPoint;

	// 점수가 변경 됐을 경우, 다른 클라이언트들에게 동기화
	OnRep_Point();
}

void ACollectorPlayerState::ResetPoint()
{
	Point = 0;
}

const uint8& ACollectorPlayerState::GetPoint() const
{
	return Point;
}

void ACollectorPlayerState::GainEnergy(const uint8& GainedEnergy)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to gain a Energy, there is an logic error or client cheated"));
		return;
	}

	Energy += GainedEnergy;

	// 획득 에너지 갯수가 변경 됐을 경우, 다른 클라이언트들에게 동기화
	OnRep_Energy();
}

void ACollectorPlayerState::ResetEnergy()
{
	Energy = 0;
}

const uint8& ACollectorPlayerState::GetEnergy() const
{
	return Energy;
}

void ACollectorPlayerState::GainMoney(const uint8& GainedMoney)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to gain a Money, there is an logic error or client cheated"));
		return;
	}
	
	Money += GainedMoney;

	// 획득 재화 갯수가 변경 됐을 경우, 다른 클라이언트들에게 동기화
	OnRep_Money();
}

void ACollectorPlayerState::ResetMoney()
{
	Money = 0;
}

const uint8& ACollectorPlayerState::GetMoney() const
{
	return Money;
}

void ACollectorPlayerState::OnRep_Point()
{
}

void ACollectorPlayerState::OnRep_Money()
{
}

void ACollectorPlayerState::OnRep_Energy()
{
}

void ACollectorPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectorPlayerState, Point);
	DOREPLIFETIME(ACollectorPlayerState, Money);
	DOREPLIFETIME(ACollectorPlayerState, Energy);
}
