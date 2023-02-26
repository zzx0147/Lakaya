// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFire.h"

#include "GameFramework/GameStateBase.h"

void URiffleFire::FireStart_Implementation(const float& Time)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(StartTimer)) return;

	switch (FireMode)
	{
	case EFireMode::Semi:
		FireCount = 1;
		TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, LockstepTimerTime(Time));
		break;
	case EFireMode::Burst:
		//TODO: RPC가 늦게 도착하여 InFirstDelay가 음수가 되는 경우, 즉시 시작되지 않고 InRate만큼 기다렸다가 시작되므로 수정이 필요합니다.
		FireCount = 3;
		TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, 0.1f, true, LockstepTimerTime(Time));
		break;
	case EFireMode::Auto:
		FireCount = -1;
		TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, 0.1f, true, LockstepTimerTime(Time));
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("FireMode was not EFireMode"));
	}

	TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, LockstepTimerTime(Time));
}

void URiffleFire::FireStop_Implementation(const float& Time)
{
	if (FireMode != EFireMode::Auto) return;
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &URiffleFire::StopAutoFire, LockstepTimerTime(Time));
}

void URiffleFire::SwitchFireMode_Implementation(const float& Time)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(StartTimer) || TimerManager.IsTimerActive(StopTimer)) return;

	switch (DesiredFireMode)
	{
	case EFireMode::Semi: DesiredFireMode = EFireMode::Burst;
		break;
	case EFireMode::Burst: DesiredFireMode = EFireMode::Auto;
		break;
	case EFireMode::Auto: DesiredFireMode = EFireMode::Semi;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("DesiredFire was not EFireMode"));
	}

	TimerManager.SetTimer(SwitchModeTimer, this, &URiffleFire::UpdateFireMode, LockstepTimerTime(Time));
}

float URiffleFire::LockstepTimerTime(const float& Time) const
{
	return Time + LockstepDelay - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void URiffleFire::TraceFire()
{
	if (FireCount-- == 0) GetWorld()->GetTimerManager().ClearTimer(StartTimer);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White,TEXT("Fire"));
}

void URiffleFire::StopAutoFire()
{
	GetWorld()->GetTimerManager().ClearTimer(StartTimer);
}

void URiffleFire::UpdateFireMode()
{
	FireMode = DesiredFireMode;
}
