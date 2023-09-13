// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Projectile.h"

#include "AbilitySystemComponent.h"
#include "Algo/Accumulate.h"
#include "Character/Ability/Actor/LakayaProjectile.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

FProjectilePoolItem::FProjectilePoolItem(ALakayaProjectile* InProjectile,
                                         const FProjectileStateChanged::FDelegate& Delegate) : Projectile(InProjectile)
{
	BindProjectileItem(Delegate);
}

void FProjectilePoolItem::BindProjectileItem(const FProjectileStateChanged::FDelegate& Delegate)
{
	if (!ensure(Projectile && Delegate.IsBound()))
	{
		return;
	}

	OnProjectileStateChangedHandle = Projectile->OnProjectileStateChanged.Add(Delegate);

	// Execute for initial state
	Delegate.Execute(Projectile, {}, Projectile->GetProjectileState());
}

void FProjectilePoolItem::UnbindProjectileItem()
{
	if (OnProjectileStateChangedHandle.IsValid())
	{
		if (Projectile)
		{
			Projectile->OnProjectileStateChanged.Remove(OnProjectileStateChangedHandle);
		}
		OnProjectileStateChangedHandle.Reset();
	}
}

void FProjectilePoolItem::PostReplicatedAdd(const FProjectilePool& InArray)
{
	// We does not modify 'Items' so It's safe to cast away const.
	auto& CastedArray = const_cast<FProjectilePool&>(InArray);
	BindProjectileItem(CastedArray.CreateClientProjectileStateDelegate());
}

void FProjectilePool::Initialize(UWorld* InSpawnWorld, const FActorSpawnParameters& InActorSpawnParameters)
{
	SpawnWorld = InSpawnWorld;
	ActorSpawnParameters = InActorSpawnParameters;
	ReFeelExtraObjects();
}

bool FProjectilePool::IsMaximumReached() const
{
	return MaxPoolSize != 0 && Items.Num() >= MaxPoolSize;
}

bool FProjectilePool::IsExtraObjectMaximumReached() const
{
	return FreeProjectiles.Num() >= MaxExtraObjectCount;
}

void FProjectilePool::AddNewObject()
{
	if (!IsMaximumReached() && !IsExtraObjectMaximumReached())
	{
		InternalAddNewObject();
	}
}

FProjectilePool::~FProjectilePool()
{
	for (auto&& Item : Items)
	{
		Item.UnbindProjectileItem();
	}
}

void FProjectilePool::InternalAddNewObject()
{
	const auto Instance = SpawnWorld->SpawnActor<ALakayaProjectile>(ProjectileClass, ActorSpawnParameters);
	if (!ensure(Instance))
	{
		return;
	}
	Instance->SetReplicates(true);

	auto& Item = Items[Items.Emplace(Instance, CreateServerProjectileStateDelegate())];
	MarkItemDirty(Item);
}

void FProjectilePool::ReFeelExtraObjects()
{
	const auto RemainExtraCount = MaxExtraObjectCount - FreeProjectiles.Num();
	const auto FixedSpawnCount = MaxPoolSize > 0
		                             ? FMath::Min(RemainExtraCount, MaxPoolSize - Items.Num())
		                             : RemainExtraCount;

	for (auto Count = 0; Count < FixedSpawnCount; ++Count)
	{
		InternalAddNewObject();
	}
}

void FProjectilePool::ClientProjectileStateChanged(ALakayaProjectile* InProjectile, const FProjectileState& OldState,
                                                   const FProjectileState& NewState)
{
	NewState.IsCollapsed() ? FreeProjectiles.AddUnique(InProjectile) : FreeProjectiles.Remove(InProjectile);
}

void FProjectilePool::ServerProjectileStateChanged(ALakayaProjectile* InProjectile, const FProjectileState& OldState,
                                                   const FProjectileState& NewState)
{
	if (NewState.IsCollapsed())
	{
		FreeProjectiles.AddUnique(InProjectile);
	}
	else
	{
		FreeProjectiles.Remove(InProjectile);
		AddNewObject();
	}
}

ALakayaProjectile* FProjectilePool::GetFreeProjectile()
{
	static const auto GetOlderProjectile = [](ALakayaProjectile* A, ALakayaProjectile* B)
	{
		return A->GetRecentPerformedTime() < B->GetRecentPerformedTime() ? A : B;
	};

	static const auto ProjectileReduce = [](ALakayaProjectile* Recent, const FProjectilePoolItem& Item)
	{
		return !Recent ? Item.Projectile : !Item.Projectile ? Recent : GetOlderProjectile(Recent, Item.Projectile);
	};

	ALakayaProjectile* Projectile = nullptr;

	if (FreeProjectiles.IsEmpty())
	{
		// 투사체는 실제로 투사체가 사용되기 시작할 때 생성되며 여기에서 생성하지 않습니다.
		if (NoExtraPolicy == EPoolNoObjectPolicy::RecycleOldest)
		{
			// 로컬 클라이언트에서 예측적으로 동작하는 것일 수도 있으므로 여기에서 투사체를 Collapse 시키지는 않습니다.
			Projectile = Algo::Accumulate(Items, Projectile, ProjectileReduce);
		}
	}
	else
	{
		Projectile = FreeProjectiles.Top().Get();
	}

	return Projectile;
}

ULakayaAbility_Projectile::ULakayaAbility_Projectile(): ProjectilePool()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void ULakayaAbility_Projectile::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActorInfo->IsNetAuthority())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = ActorInfo->OwnerActor.Get();
		SpawnParameters.Instigator = Cast<APawn>(ActorInfo->AvatarActor.Get());
		ProjectilePool.Initialize(GetWorld(), SpawnParameters);
	}
}

void ULakayaAbility_Projectile::OnTargetDataReceived_Implementation(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag)
{
	Super::OnTargetDataReceived_Implementation(TargetDataHandle, GameplayTag);

	const static auto StructName = FGameplayAbilityTargetData_ThrowProjectile::StaticStruct()->GetStructCPPName();

	if (ensure(TargetDataHandle.IsValid(0)))
	{
		const auto RawTargetDataPtr = TargetDataHandle.Get(0);
		if (ensure(RawTargetDataPtr->GetScriptStruct()->GetStructCPPName() == StructName))
		{
			const auto TargetData = static_cast<const FGameplayAbilityTargetData_ThrowProjectile*>(RawTargetDataPtr);
			if (ensure(TargetData->Projectile))
			{
				TargetData->Projectile->ThrowProjectile(TargetData->ThrowData);
				return;
			}
		}
	}

	K2_CancelAbility();
}

FGameplayAbilityTargetDataHandle ULakayaAbility_Projectile::MakeTargetData_Implementation()
{
	const auto NewProjectile = ProjectilePool.GetFreeProjectile();
	check(IsValid(NewProjectile))

	static FVector ThrowLocation, ThrowDirection;
	MakeProjectileThrowLocation(ThrowLocation, ThrowDirection);

	const auto TargetData = new FGameplayAbilityTargetData_ThrowProjectile(
		{ThrowLocation, ThrowDirection, static_cast<float>(GetWorld()->GetGameState()->GetServerWorldTimeSeconds())},
		NewProjectile);

	NewProjectile->
		ThrowProjectile(TargetData->ThrowData, GetAbilitySystemComponentFromActorInfo_Checked()->ScopedPredictionKey);

	FGameplayAbilityTargetDataHandle TargetDataHandle(TargetData);
	return TargetDataHandle;
}

void ULakayaAbility_Projectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULakayaAbility_Projectile, ProjectilePool);
}
