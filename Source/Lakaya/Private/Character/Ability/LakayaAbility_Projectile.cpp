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

bool FProjectilePoolItem::BindProjectileItem(const FProjectileStateChanged::FDelegate& Delegate)
{
	check(Delegate.IsBound())
	if (!IsValid(Projectile))
	{
		return false;
	}

	OnProjectileStateChangedHandle = Projectile->OnProjectileStateChanged.Add(Delegate);

	// Execute for initial state
	Delegate.Execute(Projectile, {}, Projectile->GetProjectileState());
	return true;
}

void FProjectilePoolItem::UnbindProjectileItem()
{
	if (OnProjectileStateChangedHandle.IsValid())
	{
		if (IsValid(Projectile))
		{
			Projectile->OnProjectileStateChanged.Remove(OnProjectileStateChangedHandle);
		}
		OnProjectileStateChangedHandle.Reset();
	}
}

void FProjectilePoolItem::PostReplicatedAdd(const FProjectilePool& InArray)
{
	if (!BindProjectileItem(InArray.CreateClientProjectileStateDelegate()))
	{
		InArray.PendingBindItems.Emplace(this);
	}
}

void FProjectilePoolItem::PreReplicatedRemove(const FProjectilePool& InArray)
{
	InArray.FreeProjectiles.RemoveSwap(Projectile);
}

void FProjectilePool::Initialize(UWorld* InSpawnWorld, const FActorSpawnParameters& InActorSpawnParameters)
{
	SpawnWorld = InSpawnWorld;
	ActorSpawnParameters = InActorSpawnParameters;
	ReFeelExtraObjects();
}

bool FProjectilePool::IsAvailable() const
{
	switch (NoExtraPolicy)
	{
	case EPoolNoObjectPolicy::ReturnNull:
		return !FreeProjectiles.IsEmpty();
	case EPoolNoObjectPolicy::RecycleOldest:
		return !Items.IsEmpty();
	default: return false;
	}
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
	check(Instance)
	Instance->SetReplicates(true);
	OnProjectileSpawned.Broadcast(Instance);

	auto& Item = Items[Items.Emplace(Instance, CreateServerProjectileStateDelegate())];
	MarkItemDirty(Item);
}

void FProjectilePool::ReFeelExtraObjects()
{
	const auto RemainExtraCount = MaxExtraObjectCount - FreeProjectiles.Num();
	const auto FixedSpawnCount = MaxPoolSize > 0
		                             ? FMath::Min(RemainExtraCount, MaxPoolSize - Items.Num())
		                             : RemainExtraCount;

	for (uint32 Count = 0; Count < FixedSpawnCount; ++Count)
	{
		InternalAddNewObject();
	}
}

void FProjectilePool::ClientProjectileStateChanged(ALakayaProjectile* InProjectile, const FProjectileState& OldState,
                                                   const FProjectileState& NewState) const
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
	else if (FreeProjectiles.Remove(InProjectile) != INDEX_NONE)
	{
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

bool FProjectilePool::RemoveProjectile(ALakayaProjectile* Projectile)
{
	if (Projectile && Items.RemoveSwap(Projectile) != INDEX_NONE)
	{
		MarkArrayDirty();
		FreeProjectiles.RemoveSwap(Projectile);
		if (Projectile->IsPendingKillPending())
		{
			OnPreProjectileDestroy.Broadcast(Projectile);
			Projectile->Destroy();
		}
		AddNewObject();
		return true;
	}
	return false;
}

void FProjectilePool::ClearProjectilePool()
{
	//TODO: 이 스코프 내에서 AddNewObject나 RemoveProjectile이 호출되면 문제가 발생합니다. ScopedListLock을 구현해야할 수 있습니다.
	for (auto&& Item : Items)
	{
		Item.UnbindProjectileItem();
		if (Item.Projectile && Item.Projectile->IsPendingKillPending())
		{
			OnPreProjectileDestroy.Broadcast(Item.Projectile);
			Item.Projectile->Destroy();
		}
	}
	Items.Empty();
	FreeProjectiles.Empty();
	MarkArrayDirty();
}

void FProjectilePool::SetInstigator(APawn* Instigator)
{
	ActorSpawnParameters.Instigator = Instigator;
	for (auto&& Item : Items)
	{
		Item.Projectile->SetInstigator(Instigator);
	}
}

ULakayaAbility_Projectile::ULakayaAbility_Projectile(): ProjectilePool()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	ProjectilePool.OnProjectileSpawned.AddUniqueDynamic(this, &ThisClass::OnProjectileSpawned);
	ProjectilePool.OnPreProjectileDestroy.AddUniqueDynamic(this, &ThisClass::OnPreProjectileDestroy);
}

void ULakayaAbility_Projectile::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActorInfo->IsNetAuthority())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = ActorInfo->OwnerActor.Get();
		SpawnParameters.Instigator = Cast<APawn>(ActorInfo->AvatarActor);
		ProjectilePool.Initialize(GetWorld(), SpawnParameters);
	}
}

void ULakayaAbility_Projectile::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	if (ActorInfo->IsNetAuthority())
	{
		ProjectilePool.SetInstigator(Cast<APawn>(ActorInfo->AvatarActor));
	}
}

void ULakayaAbility_Projectile::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	if (ActorInfo->IsNetAuthority())
	{
		ProjectilePool.ClearProjectilePool();
	}
}

bool ULakayaAbility_Projectile::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayTagContainer* SourceTags,
                                                   const FGameplayTagContainer* TargetTags,
                                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	// 사용 가능한 투사체가 있을 때만 어빌리티를 사용할 수 있도록 합니다.
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
		&& ProjectilePool.IsAvailable();
}

void ULakayaAbility_Projectile::OnTargetDataReceived_Implementation(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag)
{
	Super::OnTargetDataReceived_Implementation(TargetDataHandle, GameplayTag);

	if (ensure(TargetDataHandle.IsValid(0)))
	{
		const auto RawTargetDataPtr = TargetDataHandle.Get(0);
		if (ensure(RawTargetDataPtr->GetScriptStruct() == FGameplayAbilityTargetData_ThrowProjectile::StaticStruct()))
		{
			const auto TargetData = static_cast<const FGameplayAbilityTargetData_ThrowProjectile*>(RawTargetDataPtr);
			if (ensure(TargetData->Projectile))
			{
				TargetData->Projectile->ThrowProjectileAuthoritative(TargetData->ThrowData);
				return;
			}
		}
	}

	K2_CancelAbility();
}

FGameplayAbilityTargetDataHandle ULakayaAbility_Projectile::MakeTargetData_Implementation()
{
	const auto NewProjectile = ProjectilePool.GetFreeProjectile();
	if (!ensure(IsValid(NewProjectile)))
	{
		//TODO: 일단 크래시가 발생하지는 않도록 임시조치를 진행했습니다.
		return {};
	}

	FVector ThrowLocation, ThrowDirection;
	MakeProjectileThrowLocation(ThrowLocation, ThrowDirection);

	const auto TargetData = new FGameplayAbilityTargetData_ThrowProjectile(
		{ThrowLocation, ThrowDirection, static_cast<float>(GetWorld()->GetGameState()->GetServerWorldTimeSeconds())},
		NewProjectile);

	NewProjectile->
		ThrowProjectile(TargetData->ThrowData, GetAbilitySystemComponentFromActorInfo_Checked()->ScopedPredictionKey);

	FGameplayAbilityTargetDataHandle TargetDataHandle(TargetData);
	return TargetDataHandle;
}

void ULakayaAbility_Projectile::OnProjectileSpawned(ALakayaProjectile* Projectile)
{
	BP_Log(FString::Printf(TEXT("%s Spawned"), *Projectile->GetName()));
	Projectile->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnProjectileDestroyed);
}

void ULakayaAbility_Projectile::OnPreProjectileDestroy(ALakayaProjectile* Projectile)
{
	// 풀 내부에서 삭제하는 투사체에 대해서는 OnProjectileDestroyed가 호출되지 않도록 합니다.
	Projectile->OnDestroyed.RemoveAll(this);
}

void ULakayaAbility_Projectile::OnProjectileDestroyed(AActor* Projectile)
{
	BP_Log(FString::Printf(TEXT("%s external destroyed!"), Projectile ? *Projectile->GetName() : TEXT("")));
	ProjectilePool.RemoveProjectile(Cast<ALakayaProjectile>(Projectile));
}

void ULakayaAbility_Projectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULakayaAbility_Projectile, ProjectilePool);
}
