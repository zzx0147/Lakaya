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
	Ar << Projectile << ThrowData;
	bOutSuccess = true;
	return true;
}

ALakayaProjectile::ALakayaProjectile()
{
	SetReplicates(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
}

void ALakayaProjectile::ThrowProjectilePredictive(FPredictionKey& Key, const FProjectileThrowData& InThrowData)
{
	if (!Key.IsValidKey())
	{
		UE_LOG(LogActor, Log, TEXT("[%s] ThrowProjectilePredictive has ignored because of invalid prediction key"),
		       *GetName());
		return;
	}

	// 예측 키가 Reject되는 경우 다시 서버로부터 리플리케이트되는 정보를 바탕으로 동작하도록 합니다. 
	Key.NewRejectedDelegate().BindUObject(this, &ALakayaProjectile::RejectProjectile);

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

void ALakayaProjectile::CollapseProjectile()
{
	if (!HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[%s] Only server can collapse projectile"), *GetName());
		return;
	}

	if (!IsCollapsed())
	{
		SetProjectileState(EProjectileState::Collapsed);
		OnCollapsed();
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
		BroadcastOnProjectileStateChanged(OldState, StateRef);
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
		BroadcastOnProjectileStateChanged(OldState, StateRef);
	}
}

void ALakayaProjectile::BroadcastOnProjectileStateChanged(const FProjectileState& OldState,
                                                          const FProjectileState& NewState)
{
	FScopedLock RecursiveLock(bIsStateChanging);

	//TODO: 투사체의 상태가 변경되면서 OnProjectileStateChanged에서 이벤트 바인딩을 해제하는 경우 문제가 될 수 있으므로 복사해서 실행해야할 수 있습니다.
	OnProjectileStateChanged.Broadcast(this, OldState, NewState);
}

void ALakayaProjectile::RejectProjectile()
{
	// 예측적으로 투척되며 변경되었던 데이터들을 다시 서버로부터 리플리케이트된 데이터들로 바꿉니다.
	RecentProjectilePerformedTime = ThrowData.ServerTime;
	OnRep_ProjectileState();
}

void ALakayaProjectile::OnRep_ProjectileState()
{
	if (LocalState != ProjectileState)
	{
		const auto OldState = LocalState;
		LocalState = ProjectileState;
		BroadcastOnProjectileStateChanged(OldState, LocalState);

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
