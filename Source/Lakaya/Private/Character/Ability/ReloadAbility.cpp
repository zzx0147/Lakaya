// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ReloadAbility.h"

#include "Character/BulletComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Net/UnrealNetwork.h"


void UReloadAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UReloadAbility, bIsReloading, COND_SkipOwner);
}

UReloadAbility::UReloadAbility()
{
	ReloadDelay = 5.f;
}

void UReloadAbility::AbilityStart()
{
	if (bIsReloading) return;

	// 서버인 경우 그냥 RPC를 실행합니다.
	if (GetOwner()->HasAuthority())
	{
		Super::AbilityStart();
		return;
	}

	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(OwnerTimer, [this]
	{
		bIsReloading = false;
		OnReloadStateChanged.Broadcast(bIsReloading);
	}, ReloadDelay, false);
	Super::AbilityStart();
	OnReloadStateChanged.Broadcast(bIsReloading);
}

void UReloadAbility::BeginPlay()
{
	Super::BeginPlay();
	if (const auto LocalOwner = GetOwner())
		if (LocalOwner->HasAuthority())
			if (const auto Character = Cast<ALakayaBaseCharacter>(LocalOwner))
				BulletComponent = Character->GetResourceComponent<UBulletComponent>();
			else UE_LOG(LogInit, Error, TEXT("Fail to find BulletComponent!"));
}

void UReloadAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (bIsReloading) return;
	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(OwnerTimer, [this]
	{
		bIsReloading = false;
		OnReloadStateChanged.Broadcast(bIsReloading);
		if (BulletComponent.IsValid()) BulletComponent->Reload();
		else UE_LOG(LogActorComponent, Error, TEXT("BulletComponent is invalid!"));
	}, ReloadDelay, false);
	OnReloadStateChanged.Broadcast(bIsReloading);
}

void UReloadAbility::OnRep_IsReloading()
{
	OnReloadStateChanged.Broadcast(bIsReloading);
}
