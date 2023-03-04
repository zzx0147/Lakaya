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
	if (TimerManager.IsTimerActive(StartTimer)) return;

	auto ReservedTime = LockstepTimerTime(Time);
	if (ReservedTime < 0.f) return;

	FireStartNotify(Time);

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
	//TODO: InRate에 발사속도를 기입합니다.
	TimerManager.SetTimer(StartTimer, this, &URiffleFire::TraceFire, 0.1f, true, ReservedTime);
}

void URiffleFire::FireStop_Implementation(const float& Time)
{
	if (FireMode != EFireMode::Auto) return;
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

	//TODO: 재장전에 걸리는 시간을 기입합니다.
	TimerManager.SetTimer(SwitchModeTimer, this, &URiffleFire::UpdateFireMode, LockstepTimerTime(Time));
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
	//TODO: 받아온 데이터를 적용합니다.
}

float URiffleFire::LockstepTimerTime(const float& Time) const
{
	return Time + LockstepDelay - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void URiffleFire::TraceFire()
{
	if (--FireCount == 0) StopFire();

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

	UGameplayStatics::ApplyDamage(HitResult.GetActor(), 10.f, Character->GetController(),
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
