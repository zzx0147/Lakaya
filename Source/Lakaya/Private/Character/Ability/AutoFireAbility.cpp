// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AutoFireAbility.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UAutoFireAbility::UAutoFireAbility()
{
	bWantsInitializeComponent = true;
	AbilityStartDelay = AbilityStopDelay = 0.2f;
	bUpdateStartTimeOnStart = bUpdateStopTimeOnStop = true;
	FireDelay = 0.2f;
	FireRange = 5000.f;
	FireDamage = 20.f;
	bCanEverStopRemoteCall = bCanEverStartRemoteCall = true;
}

bool UAutoFireAbility::ShouldStartRemoteCall()
{
	if (GetOwner()->HasAuthority()) return true;
	if (bWantsToFire) return false;
	bWantsToFire = true;
	return true;
}

bool UAutoFireAbility::ShouldStopRemoteCall()
{
	if (GetOwner()->HasAuthority()) return true;
	if (!bWantsToFire) return false;
	bWantsToFire = false;
	return true;
}

void UAutoFireAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	if (bWantsToFire) return;
	bWantsToFire = true;
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !TimerManager.TimerExists(FireTimer))
		TimerManager.SetTimer(FireTimer, this, &UAutoFireAbility::FireTick, FireDelay, true, 0.f);
}

void UAutoFireAbility::RemoteAbilityStop(const float& RequestTime)
{
	Super::RemoteAbilityStop(RequestTime);
	if (!bWantsToFire) return;
	bWantsToFire = false;
}

void UAutoFireAbility::SetBasisComponent(USceneComponent* NewComponent)
{
	BasisComponent = NewComponent;
}

bool UAutoFireAbility::ShouldFire()
{
	return true;
}

void UAutoFireAbility::SingleFire()
{
	const auto End = GetCameraForwardTracePoint(FireRange, CollisionQueryParams);
	if (FHitResult Result; GetWorld()->LineTraceSingleByChannel(Result, BasisComponent->GetComponentLocation(), End,
	                                                            ECC_Visibility, CollisionQueryParams))
	{
		const auto Pawn = GetOwner<APawn>();
		UGameplayStatics::ApplyPointDamage(Result.GetActor(), FireDamage, End, Result,
		                                   Pawn ? Pawn->GetController() : nullptr, GetOwner(), nullptr);
	}
}

void UAutoFireAbility::FailToFire()
{
	RemoteAbilityStop(GetServerTime());
}

void UAutoFireAbility::FireTick()
{
	if (!bWantsToFire) GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	else if (ShouldFire()) SingleFire();
	else FailToFire();
}

void UAutoFireAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (!AliveState && GetOwner()->HasAuthority()) RemoteAbilityStop(GetServerTime());
}

void UAutoFireAbility::InitializeComponent()
{
	Super::InitializeComponent();
	CollisionQueryParams.AddIgnoredActor(GetOwner());
}
