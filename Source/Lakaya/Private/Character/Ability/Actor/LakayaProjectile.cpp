// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Actor/LakayaProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

bool FGameplayAbilityTargetData_LocationWithTime::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	ThrowLocation.NetSerialize(Ar, Map, bOutSuccess);
	Ar << ServerTime;

	bOutSuccess = true;
	return true;
}

ALakayaProjectile::ALakayaProjectile()
{
	SetReplicates(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
}

void ALakayaProjectile::ThrowProjectilePredictive(const FPredictionKey& Key)
{
	if (!IsCollapsed())
	{
		UE_LOG(LogActor, Error, TEXT("[%s] Try to throw projectile predictive when it's not collapsed!"), *GetName());
		return;
	}
	
}

void ALakayaProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALakayaProjectile, ProjectileState);
	DOREPLIFETIME(ALakayaProjectile, CustomState);
}

void ALakayaProjectile::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	DOREPLIFETIME_ACTIVE_OVERRIDE(ALakayaProjectile, CustomState, IsCustomState());
}

void ALakayaProjectile::CustomStateChanged_Implementation(const uint8& OldState)
{
	UE_LOG(LogActor, Log, TEXT("[%s] Projectile custom state changed from %d to %d"), *GetName(), OldState,
	       GetCustomState());
}

void ALakayaProjectile::OnRep_ProjectileState()
{
	if (IsCustomState())
	{
		if (CachedCustomState == CustomState)
		{
			// ProjectileStated와 CustomState가 동시에 업데이트되어 두번 호출된 경우이므로 스킵합니다.
			return;
		}
		const auto OldCustomState = CachedCustomState;
		CachedCustomState = CustomState;
		CustomStateChanged(OldCustomState);
	}
	else
	{
		CachedCustomState = 0;
	}
}
