// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ReloadAbility.h"

#include "Character/BulletComponent.h"
#include "Net/UnrealNetwork.h"


void UReloadAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UReloadAbility, ReloadingTime);
}

UReloadAbility::UReloadAbility()
{
	ReloadDelay = 2.3f;
	bCanEverStartRemoteCall = true;
}

void UReloadAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (!AliveState && GetOwner()->HasAuthority())
	{
		ReloadingTime = 0.f;
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
		SetReloadState(false);
	}
}

void UReloadAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);

	// 재장전중이지 않고, BulletComponent가 존재하고, 탄창이 가득차있지 않고, 살아있는 경우에만 재장전을 시작합니다.
	if (const auto BulletComponent = GetResourceComponent<UBulletComponent>();
		bRecentReloadState || !BulletComponent || BulletComponent->IsFull() || !GetAliveState())
		return;

	ReloadingTime = GetServerTime() + ReloadDelay;
	SetReloadState(true);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UReloadAbility::ReloadTimerHandler, ReloadDelay);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("ReloadTimerSetted!"));
}

bool UReloadAbility::ShouldStartRemoteCall()
{
	// 재장전 완료시점까지 남은 시간이 0.1초 미만인 경우 서버에 재장전 시작 요청을 할 수 있도록 합니다.
	return ReloadingTime - GetServerTime() < 0.1f;
}

void UReloadAbility::ReloadTimerHandler()
{
	if (GetOwner()->HasAuthority())
	{
		GetResourceComponent<UBulletComponent>()->Reload();
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Reloaded!"));
	}
	SetReloadState(false);
}

void UReloadAbility::OnRep_ReloadingTime()
{
	const auto CurrentTime = GetServerTime();
	UpdateReloadStateWithTime(CurrentTime);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UReloadAbility::ReloadTimerHandler,
	                                       ReloadingTime - CurrentTime);
}

void UReloadAbility::SetReloadState(const bool& NewState)
{
	if (NewState == bRecentReloadState) return;
	bRecentReloadState = NewState;
	OnReloadStateChanged.Broadcast(bRecentReloadState);
}

void UReloadAbility::UpdateReloadStateWithTime(const float& CurrentTime)
{
	SetReloadState(ReloadingTime > CurrentTime);
}
