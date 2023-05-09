// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ReloadAbility.h"

#include "Character/BulletComponent.h"
#include "Net/UnrealNetwork.h"


void UReloadAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UReloadAbility, bIsReloading);
}

UReloadAbility::UReloadAbility()
{
	ReloadDelay = 5.f;
}

void UReloadAbility::AbilityStart()
{
	if (!bIsReloading) Super::AbilityStart();
}

void UReloadAbility::BeginPlay()
{
	Super::BeginPlay();
	if (const auto Actor = GetOwner(); Actor && Actor->HasAuthority())
	{
		BulletComponent = Actor->FindComponentByClass<UBulletComponent>();
		if (!BulletComponent.IsValid()) UE_LOG(LogInit, Error, TEXT("Fail to find BulletComponent!"));
	}
}

void UReloadAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);

	// 재장전중이지 않고, BulletComponent가 존재하고, 탄창이 가득차있지 않는 경우에만 재장전을 시작합니다.
	if (bIsReloading || !BulletComponent.IsValid() || BulletComponent->IsFull()) return;

	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(OwnerTimer, this, &UReloadAbility::ReloadTimerHandler, ReloadDelay);
	OnReloadStateChanged.Broadcast(bIsReloading);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("ReloadTimerSetted!"));
}

void UReloadAbility::OnRep_IsReloading()
{
	OnReloadStateChanged.Broadcast(bIsReloading);
}

void UReloadAbility::ReloadTimerHandler()
{
	bIsReloading = false;
	if (BulletComponent.IsValid())
	{
		BulletComponent->Reload();
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Reloaded!"));
	}
	else UE_LOG(LogActorComponent, Error, TEXT("BulletComponent is invalid!"));
	OnReloadStateChanged.Broadcast(bIsReloading);
}
