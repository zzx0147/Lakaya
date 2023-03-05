// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFire.h"

#include "ThirdPersonCharacter.h"
#include "WeaponFireData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"

URiffleFire::URiffleFire()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponFireDataTable'"));

	if (TableFinder.Succeeded()) WeaponFireDataTable = TableFinder.Object;
}

void URiffleFire::FireStart_Implementation(const float& Time)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	auto ReservedTime = LockstepTimerTime(Time);
	if (ReservedTime < 0.f || TimerManager.IsTimerActive(SelectorTimer)) return;

	if (TimerManager.IsTimerActive(StartTimer))
	{
		// if (FireMode == EFireMode::Auto && Time > LastStartTime) return;
		//
		// // Continuous firing
		// if (FireCount < 1 && FireMode != EFireMode::Auto)
		// {
		// 	FireCount = FireMode == EFireMode::Semi ? 1 : 3;
		// 	FireStartNotify(Time);
		// 	return;
		// }
		return;
	}

	FireStartNotify(Time);

	switch (FireMode)
	{
	case EFireMode::Semi: FireCount = 1;
		break;
	case EFireMode::Burst: FireCount = 3;
		break;
	case EFireMode::Auto: FireCount = -1;
		LastStartTime = Time;
		if (Time > LastStopTime) TimerManager.ClearTimer(StopTimer);
		break;
	default: UE_LOG(LogActorComponent, Error, TEXT("FireMode was not EFireMode"));
		return;
	}

	TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, FireDelay, true, ReservedTime);
}

void URiffleFire::FireStop_Implementation(const float& Time)
{
	if (FireMode != EFireMode::Auto || Time < LastStartTime || Time < LastStopTime) return;
	LastStopTime = Time;
	FireStopNotify(Time);
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &URiffleFire::StopFire, LockstepTimerTime(Time));
}

void URiffleFire::SwitchSelector_Implementation(const float& Time)
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

	TimerManager.SetTimer(SelectorTimer, this, &URiffleFire::UpdateFireMode,
	                      Time + SwitchingDelay - GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}

void URiffleFire::FireStartNotify_Implementation(const float& Time)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStart"));
}

void URiffleFire::FireStopNotify_Implementation(const float& Time)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStop"));
}

void URiffleFire::SetupData_Implementation(const FName& RowName)
{
	auto Component = Cast<UActorComponent>(GetOuter());
	if (Component == nullptr)
	{
		UE_LOG(LogNetSubObject, Error, TEXT("Outer was not UActorComponent!"));
		return;
	}

	Character = Cast<AThirdPersonCharacter>(Component->GetOwner());
	if (Character.IsStale())
	{
		UE_LOG(LogNetSubObject, Error, TEXT("Attached Actor was not AThirdPersonCharacter."));
		return;
	}

	TraceQueryParams.AddIgnoredActor(Character.Get());

	auto Data = WeaponFireDataTable->FindRow<FWeaponFireData>(RowName,TEXT("RiffleFire"));
	BaseDamage = Data->BaseDamage;
	FireDelay = 60 / Data->FireRate;
	FireRange = Data->FireRange;
	SwitchingDelay = Data->SelectorSwitchingDelay;
}

float URiffleFire::LockstepTimerTime(const float& Time) const
{
	return Time + LockstepDelay - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void URiffleFire::TraceFire()
{
	if (FireCount-- == 0)
	{
		StopFire();
		return;
	}

	//TODO: 사거리를 제한하는 로직을 추가합니다.
	FHitResult HitResult;
	const auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	const auto CameraForward = CameraLocation + Character->GetCamera()->GetForwardVector() * 10000;
	FVector AimPoint;

	DrawDebugLine(GetWorld(), CameraLocation, CameraForward, FColor::Green, false, 1);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraForward, ECC_Camera, TraceQueryParams))
		AimPoint = HitResult.ImpactPoint;
	else AimPoint = CameraForward;

	const auto& Location = Character->GetActorLocation();
	DrawDebugLine(GetWorld(), Location, AimPoint, FColor::Red, false, 1);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, AimPoint, ECC_GameTraceChannel2, TraceQueryParams))
		return;

	UGameplayStatics::ApplyDamage(HitResult.GetActor(), BaseDamage, Character->GetController(),
	                              Character.Get(), nullptr);
}

void URiffleFire::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(StartTimer);
}

void URiffleFire::UpdateFireMode()
{
	FireMode = DesiredFireMode;
}
