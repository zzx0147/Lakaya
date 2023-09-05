// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Actor/LakayaProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

bool FProjectileState::SetProjectileState(const EProjectileState& InProjectileState)
{
	// (Custom, 0) 상태가 될 수 없도록 합니다.
	if (ensure(InProjectileState != EProjectileState::Custom) && InProjectileState != ProjectileState)
	{
		CustomState = 0;
		ProjectileState = InProjectileState;
		return true;
	}
	return false;
}

bool FProjectileState::SetCustomState(const uint8& InCustomState)
{
	// (Custom, 0) 상태가 될 수 없도록 합니다.
	if (ensure(InCustomState != 0) && InCustomState != CustomState)
	{
		ProjectileState = EProjectileState::Custom;
		CustomState = InCustomState;
		return true;
	}
	return false;
}

bool FProjectileState::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << ProjectileState;

	if (ProjectileState == EProjectileState::Custom)
	{
		// ProjectileState가 Custom일 때만 전송되도록 합니다.
		Ar << CustomState;
	}
	else if (!Ar.IsSaving())
	{
		// 받는 쪽이고 ProjectileState가 Custom이 아닌 경우 CustomState를 0으로 초기화합니다.
		CustomState = 0;
	}

	bOutSuccess = true;
	return true;
}

bool FGameplayAbilityTargetData_ThrowProjectile::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Projectile << ThrowData.ThrowLocation << ThrowData.ThrowDirection << ThrowData.ServerTime;
	bOutSuccess = true;
	return true;
}

ALakayaProjectile::ALakayaProjectile()
{
	SetReplicates(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
}

void ALakayaProjectile::ThrowProjectilePredictive(const FProjectileThrowData& InThrowData)
{
	SetProjectileState(EProjectileState::Perform);
	ThrowProjectile(InThrowData);
}

void ALakayaProjectile::ThrowProjectileAuthoritative(FProjectileThrowData&& InThrowData)
{
	if (ensure(HasAuthority()))
	{
		ThrowData = MoveTemp(InThrowData);
		SetProjectileState(EProjectileState::Perform);
		ThrowProjectile(ThrowData);
		//TODO: 서버에서는 충돌검사를 수행하므로 충돌검사를 활성화시키는 로직도 필요합니다.
	}
}

void ALakayaProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALakayaProjectile, ProjectileState);
	DOREPLIFETIME_CONDITION(ALakayaProjectile, ThrowData, COND_Custom);
}

void ALakayaProjectile::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	DOREPLIFETIME_ACTIVE_OVERRIDE(ALakayaProjectile, ThrowData, IsActualPerforming());
}

void ALakayaProjectile::ThrowProjectile(const FProjectileThrowData& InThrowData)
{
	RecentProjectilePerformedTime = InThrowData.ServerTime;
	//TODO
}

void ALakayaProjectile::SetCustomState(const uint8& InCustomState)
{
	auto& StateRef = GetProjectileState();
	const auto OldState = StateRef;
	if (StateRef.SetCustomState(InCustomState))
	{
		OnProjectileStateChanged.Broadcast(this, OldState, StateRef);
	}
}

void ALakayaProjectile::OnRep_CustomState()
{
	UE_LOG(LogActor, Log, TEXT("[%s] Custom state replicated : %d"), *GetName(), GetProjectileState().GetCustomState());
}

void ALakayaProjectile::OnCollapsed_Implementation()
{
	//TODO
}

void ALakayaProjectile::SetProjectileState(const EProjectileState& InProjectileState)
{
	auto& StateRef = GetProjectileState();
	const auto OldState = StateRef;
	if (StateRef.SetProjectileState(InProjectileState))
	{
		OnProjectileStateChanged.Broadcast(this, OldState, StateRef);
	}
}

void ALakayaProjectile::OnRep_ProjectileState()
{
	if (LocalState != ProjectileState)
	{
		const auto OldState = LocalState;
		LocalState = ProjectileState;
		OnProjectileStateChanged.Broadcast(this, OldState, LocalState);

		switch (LocalState.GetProjectileState())
		{
		case EProjectileState::Collapsed:
			OnCollapsed();
			break;
		case EProjectileState::Perform:
			ThrowProjectile(ThrowData);
			break;
		case EProjectileState::Custom:
			OnRep_CustomState();
			break;
		default:
			UE_LOG(LogActor, Fatal, TEXT("[%s] Invalid ProjectileState has replicated: %d"), *GetName(),
			       LocalState.GetProjectileState());
		}
	}
}
