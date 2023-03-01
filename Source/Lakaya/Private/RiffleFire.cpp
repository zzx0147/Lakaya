// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFire.h"

#include "ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameStateBase.h"

void URiffleFire::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AThirdPersonCharacter>(GetOwner());
	if (Character.IsStale()) UE_LOG(LogActorComponent, Error, TEXT("Attached Actor was not AThirdPersonCharacter."));
	TraceQueryParams.AddIgnoredActor(GetOwner());
}

void URiffleFire::FireStart_Implementation(const float& Time)
{
	Execute_FireStartConfirmed(this, Time);
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(StartTimer)) return;

	switch (FireMode)
	{
	case EFireMode::Semi: FireCount = 1;
		break;
	case EFireMode::Burst: FireCount = 3;
		break;
	case EFireMode::Auto: FireCount = -1;
		break;
	default: UE_LOG(LogActorComponent, Error, TEXT("FireMode was not EFireMode"));
		break;
	}
	//TODO: RPC가 늦게 도착하여 InFirstDelay가 음수가 되는 경우, 즉시 시작되지 않고 InRate만큼 기다렸다가 시작되므로 수정이 필요합니다.
	//TODO: InRate에 발사속도를 기입합니다.
	TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, 0.1f, true, LockstepTimerTime(Time));
}

void URiffleFire::FireStop_Implementation(const float& Time)
{
	Execute_FireStopConfirmed(this, Time);
	if (FireMode != EFireMode::Auto) return;
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &URiffleFire::StopFire, LockstepTimerTime(Time));
}

void URiffleFire::SwitchFireMode_Implementation(const float& Time)
{
	Execute_SwitchFireModeConfirmed(this, Time);
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

void URiffleFire::FireStartConfirmed_Implementation(const float& Time)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStart"));
}

void URiffleFire::FireStopConfirmed_Implementation(const float& Time)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStop"));
}

void URiffleFire::SwitchFireModeConfirmed_Implementation(const float& Time)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Switch"));
}

float URiffleFire::LockstepTimerTime(const float& Time) const
{
	return Time + LockstepDelay - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void URiffleFire::TraceFire()
{
	if (--FireCount == 0) StopFire();

	FHitResult HitResult;
	const auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	const auto CameraForward = CameraLocation + Character->GetCamera()->GetForwardVector() * 10000;
	FVector AimPoint;

	DrawDebugLine(GetWorld(), CameraLocation, CameraForward, FColor::Green, false, 1);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraForward, ECC_Camera, TraceQueryParams))
		AimPoint = HitResult.ImpactPoint;
	else AimPoint = CameraForward;

	const auto& Location = GetOwner()->GetActorLocation();
	DrawDebugLine(GetWorld(), Location, AimPoint, FColor::Red, false, 1);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, AimPoint, ECC_GameTraceChannel2, TraceQueryParams))
		return;

	//TODO: 충돌한 폰 또는 폰의 OwnerPlayer에게 피해를 입힙니다.
}

void URiffleFire::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(StartTimer);
}

void URiffleFire::UpdateFireMode()
{
	FireMode = DesiredFireMode;
}
