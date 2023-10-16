// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Actor/LakayaProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

void FProjectileThrowData::SetupPredictedProjectileParams(FPredictProjectilePathParams& OutParams,
                                                          const float& Velocity, const float& CurrentTime) const
{
	OutParams.StartLocation = ThrowLocation;
	OutParams.LaunchVelocity = ThrowDirection * Velocity;
	OutParams.MaxSimTime = CurrentTime - ServerTime;
}

bool FGameplayAbilityTargetData_ThrowProjectile::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Projectile << ThrowData;
	bOutSuccess = true;
	return true;
}

ALakayaProjectile::ALakayaProjectile()
{
	bReplicates = true;

	// PredictProjectilePath가 radius 속성을 갖는 투사체만 지원하므로 투사체는 구체여야 합니다..
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	// 정적 오브젝트에 대해서만 충돌이 가능하도록 설정합니다. 하위 클래스에서 필요한 콜리전 채널에 대해 Overlap을 설정하는 것은 가능합니다.
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::CallOnProjectileOverlap);

	ProjectileMovementComponent =
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->OnProjectileBounce.AddUniqueDynamic(this, &ThisClass::OnProjectileBounce);
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->bAutoActivate = false;

	ProjectileLaunchVelocity = 1000.f;
}

void ALakayaProjectile::ThrowProjectile(const FProjectileThrowData& InThrowData, FPredictionKey Key)
{
	if (HasAuthority())
	{
		ThrowProjectileAuthoritative(InThrowData);
		return;
	}

	if (!Key.IsValidKey())
	{
		UE_LOG(LogActor, Log, TEXT("[%s] ThrowProjectile has ignored : invalid prediction key"), *GetName());
		return;
	}

	// 예측 키가 Reject되는 경우 다시 서버로부터 리플리케이트되는 정보를 바탕으로 동작하도록 합니다. 
	Key.NewRejectedDelegate().BindUObject(this, &ThisClass::RejectProjectile);

	SetProjectileState(EProjectileState::Perform);
	ThrowProjectile(InThrowData);
}

void ALakayaProjectile::ThrowProjectileAuthoritative(const FProjectileThrowData& InThrowData)
{
	if (ensure(HasAuthority()))
	{
		ThrowData = InThrowData;
		SetProjectileState(EProjectileState::Perform);
		ThrowProjectile(ThrowData);
	}
}

const FProjectileState& ALakayaProjectile::GetProjectileState() const
{
	return HasAuthority() ? ProjectileState : LocalState;
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

	// Collapsed에서는 RecentPerformedTime이 필요없지만 다른 상태에서는 필요합니다.
	DOREPLIFETIME_ACTIVE_OVERRIDE(ALakayaProjectile, ThrowData, !IsActualCollapsed());
}

void ALakayaProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CollisionComponent->SetGenerateOverlapEvents(HasAuthority());
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PredictedProjectileParams.OverrideGravityZ = ProjectileMovementComponent->ShouldApplyGravity()
		                                             ? ProjectileMovementComponent->GetGravityZ()
		                                             : -1.f;
	PredictedProjectileParams.ProjectileRadius = CollisionComponent->GetScaledSphereRadius();
}

void ALakayaProjectile::SetProjectileState(const EProjectileState& InProjectileState)
{
	InternalSetProjectileState(&FProjectileState::SetProjectileState, InProjectileState);
}

void ALakayaProjectile::ThrowProjectile(const FProjectileThrowData& InThrowData)
{
	LocalThrowData = InThrowData;
	InThrowData.SetupPredictedProjectileParams(PredictedProjectileParams, ProjectileLaunchVelocity,
	                                           GetServerWorldTimeSeconds());

	static FPredictProjectilePathResult Result;
	auto Location = PredictedProjectileParams.StartLocation;
	auto Velocity = PredictedProjectileParams.LaunchVelocity;
	auto EventDelay = EventTriggerDelayFromThrow;

	if (PredictedProjectileParams.MaxSimTime > 0.f)
	{
		OnStartPathPrediction(InThrowData);

		if (!MarchProjectileRecursive(Result))
		{
			return;
		}

		OnStartPhysicsSimulation(Result);

		EventDelay -= PredictedProjectileParams.MaxSimTime;
		Location = Result.LastTraceDestination.Location;
		Velocity = Result.LastTraceDestination.Velocity;
	}

	OnStartPhysicsSimulation(Result);
	GetWorldTimerManager().SetTimer(EventFromThrowTimerHandle, this, &ThisClass::OnEventFromThrowTriggeredInPhysics,
	                                EventDelay);
	ProjectileMovementComponent->Velocity = Velocity;
	ProjectileMovementComponent->Activate();
	SetActorLocation(Location);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

float ALakayaProjectile::GetServerWorldTimeSeconds() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ALakayaProjectile::SetCustomState(const uint8& InCustomState)
{
	InternalSetProjectileState(&FProjectileState::SetCustomState, InCustomState);
}

void ALakayaProjectile::SetProjectileStateCollapsed()
{
	SetProjectileState(EProjectileState::Collapsed);
}

void ALakayaProjectile::SetSourceObject(FGameplayEffectContextHandle EffectContext, const UObject* SourceObject)
{
	EffectContext.AddSourceObject(SourceObject);
}

ECollisionChannel ALakayaProjectile::ConvertToCollisionChannel(const EObjectTypeQuery& ObjectType)
{
	return UEngineTypes::ConvertToCollisionChannel(ObjectType);
}

void ALakayaProjectile::OnStartPhysicsSimulation_Implementation(
	const FPredictProjectilePathResult& LastPredictionResult)
{
	if (bIgnoreOwnerAndInstigator)
	{
		AddIgnoredInPerformActor(GetOwner());
		AddIgnoredInPerformActor(GetInstigator());
	}
}

void ALakayaProjectile::OnStartPathPrediction_Implementation(const FProjectileThrowData& InThrowData)
{
	if (bIgnoreOwnerAndInstigator)
	{
		AddIgnoredInPerformActor(GetOwner());
		AddIgnoredInPerformActor(GetInstigator());
	}
}

void ALakayaProjectile::OnReplicatedCustomStateEnter_Implementation(const uint8& NewState)
{
	UE_LOG(LogActor, Log, TEXT("[%s] Custom state Enter : %d"), *GetName(), NewState);
}

void ALakayaProjectile::OnReplicatedCustomStateExit_Implementation(const uint8& OldState)
{
	UE_LOG(LogActor, Log, TEXT("[%s] Custom state Exit : %d"), *GetName(), OldState);
}

bool ALakayaProjectile::OnProjectilePathOverlap_Implementation(const FPredictProjectilePathResult& PredictResult)
{
	return true;
}

bool ALakayaProjectile::OnProjectilePathBlock_Implementation(const FPredictProjectilePathResult& PredictResult)
{
	return true;
}

void ALakayaProjectile::OnProjectileOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                           bool bFromSweep, const FHitResult& SweepResult)
{
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
	if (LocalState == ProjectileState && LocalState.IsPerforming() && LocalThrowData != ThrowData)
	{
		// 서버에서 타겟 데이터를 거부하고 새로 투사체를 투척한 경우이므로, 간단히 투사체를 껐다가 다시 켜줍니다.
		StopThrowProjectile();
		ThrowProjectile(ThrowData);
	}
	else
	{
		// 예측적으로 투척되며 변경되었던 데이터들을 다시 서버로부터 리플리케이트된 데이터들로 바꿉니다.
		OnRep_ProjectileState();
	}
}

void ALakayaProjectile::StopThrowProjectile()
{
	if (EventFromThrowTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(EventFromThrowTimerHandle);
	}
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->Deactivate();
	ClearIgnoredInPerformActors();
}

bool ALakayaProjectile::MarchProjectileRecursive(FPredictProjectilePathResult& OutResult, float CurrentTime)
{
	// 이미 예측된 부분은 스킵하기 위해 잠시 MaxSimTime을 조정합니다.
	const auto MaxSimTime = PredictedProjectileParams.MaxSimTime;
	PredictedProjectileParams.MaxSimTime -= CurrentTime;

	const auto bIsPredictionBlocked =
		UGameplayStatics::PredictProjectilePath(this, PredictedProjectileParams, OutResult);
	PredictedProjectileParams.MaxSimTime = MaxSimTime;

	// 딜레이 이벤트 기능을 사용하고, MaxSimTime이내에 TriggerDelay가 존재하면 이벤트가 발생할 수 있는 최저 조건을 달성합니다.
	if (EventTriggerDelayFromThrow > 0.f && MaxSimTime >= EventTriggerDelayFromThrow)
	{
		const auto RemainEventTime = EventTriggerDelayFromThrow - CurrentTime;

		// 이번 예측에서 이벤트 시간을 지나쳤다면 이벤트를 발생시킵니다.
		if (RemainEventTime <= OutResult.LastTraceDestination.Time)
		{
			const auto FoundIndex = OutResult.PathData.FindLastByPredicate(
				[&RemainEventTime](const FPredictProjectilePathPointData& InData)
				{
					return InData.Time < RemainEventTime;
				});
			check(FoundIndex != INDEX_NONE);

			// FoundIndex는 항상 LastTraceDestination보다 작으므로 +1 인덱스는 안전한 접근입니다.
			const auto& FoundData = OutResult.PathData[FoundIndex];
			const auto& NextData = OutResult.PathData[FoundIndex + 1];
			const auto Alpha =
				UKismetMathLibrary::NormalizeToRange(RemainEventTime, FoundData.Time, NextData.Time);

			const auto Location = FMath::Lerp(FoundData.Location, NextData.Location, Alpha);
			const auto Velocity = FMath::Lerp(FoundData.Velocity, NextData.Velocity, Alpha);
			if (!OnEventFromThrowTriggeredInPathPredict(Location, Velocity))
			{
				return false;
			}
		}
	}

	// 충돌이 발생하지 않고 성공적으로 예측이 완료된 경우 true를 반환합니다.
	if (!bIsPredictionBlocked)
	{
		// PredictedProjectileParams에서 무시되었던 액터들을 제거합니다.
		// CollisionComponent에서 제거하지 않는 것은 아직 투사체의 Perform이 끝난 것은 아니기 때문입니다.
		for (auto&& IgnoredActor : IgnoredInPerformActors)
		{
			PredictedProjectileParams.ActorsToIgnore.RemoveSwap(IgnoredActor.Get());
		}
		return true;
	}

	// 충돌 이벤트를 처리합니다.
	const auto& HitResult = OutResult.HitResult;
	const auto HitResponse = CollisionComponent->GetCollisionResponseToComponent(HitResult.GetComponent());
	if (HitResponse == ECR_Block)
	{
		if (HitResult.bStartPenetrating)
		{
			// 너무 가까운 곳에서 투척이 발생하지 않도록 하기위한 로그입니다.
			UE_LOG(LogActor, Error, TEXT("[%s] Projectile is penetrating at start location"), *GetName());
			SetProjectileStateCollapsed();
			return false;

			//TODO: 벽이나 바닥과 같은 오브젝트에 투사체가 걸쳐져있거나, 그 안에 있습니다. 이 투사체를 꺼내서 다시 투사체를 던지도록 합니다.
		}

		if (!OnProjectilePathBlock(OutResult))
		{
			return false;
		}

		if (!ProjectileMovementComponent->bShouldBounce)
		{
			OutResult.LastTraceDestination.Velocity = FVector::ZeroVector;
			return true;
		}

		// 반사 벡터 방향으로 나아가도록 설정합니다.
		PredictedProjectileParams.LaunchVelocity =
			-OutResult.LastTraceDestination.Velocity.MirrorByVector(HitResult.ImpactNormal)
			* ProjectileMovementComponent->Bounciness;
	}
	else
	{
		if (HitResponse == ECR_Overlap && CollisionComponent->GetGenerateOverlapEvents()
			&& !OnProjectilePathOverlap(OutResult))
		{
			return false;
		}

		// 한번 Overlap 이벤트가 실행된 액터는 무시합니다.
		AddIgnoredInPerformActor(HitResult.GetActor());

		PredictedProjectileParams.LaunchVelocity = OutResult.LastTraceDestination.Velocity;
	}

	PredictedProjectileParams.StartLocation = OutResult.LastTraceDestination.Location;
	return MarchProjectileRecursive(OutResult, CurrentTime + OutResult.LastTraceDestination.Time);
}

bool ALakayaProjectile::OnEventFromThrowTriggeredInPathPredict_Implementation(const FVector& Location,
                                                                              const FVector& Velocity)
{
	return true;
}

void ALakayaProjectile::OnEventFromThrowTriggeredInPhysics_Implementation()
{
}

void ALakayaProjectile::OnProjectileBounce_Implementation(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
}

void ALakayaProjectile::PostExitPerformStateReplicated_Implementation()
{
}

bool ALakayaProjectile::ShouldRetriggerCustomState_Implementation(const uint8& CustomState)
{
	return false;
}

void ALakayaProjectile::OnCustomStateRetrigger_Implementation(const uint8& CustomState)
{
}

void ALakayaProjectile::AddIgnoredInPerformActor(AActor* InActor)
{
	if (IsValid(InActor))
	{
		PredictedProjectileParams.ActorsToIgnore.AddUnique(InActor);
		CollisionComponent->IgnoreActorWhenMoving(InActor, true);
		IgnoredInPerformActors.AddUnique(InActor);
	}
}

void ALakayaProjectile::ClearIgnoredInPerformActors()
{
	for (auto&& IgnoredActor : IgnoredInPerformActors)
	{
		CollisionComponent->IgnoreActorWhenMoving(IgnoredActor.Get(), false);
		PredictedProjectileParams.ActorsToIgnore.RemoveSwap(IgnoredActor.Get());
	}
	IgnoredInPerformActors.Reset();
}

void ALakayaProjectile::AddOverlapObjectType(const EObjectTypeQuery& ObjectType)
{
	PredictedProjectileParams.ObjectTypes.AddUnique(ObjectType);
	CollisionComponent->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(ObjectType), ECR_Overlap);
}

void ALakayaProjectile::IgnoreObjectType(const EObjectTypeQuery& ObjectType)
{
	PredictedProjectileParams.ObjectTypes.RemoveSwap(ObjectType);
	CollisionComponent->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(ObjectType), ECR_Ignore);
}

void ALakayaProjectile::OnRep_ProjectileState()
{
	if (LocalState == ProjectileState)
	{
		if (LocalState.IsCustomState() && ShouldRetriggerCustomState(LocalState.GetCustomState()))
		{
			OnCustomStateRetrigger(LocalState.GetCustomState());
		}
		return;
	}

	const auto OldState = LocalState;
	LocalState = ProjectileState;
	BroadcastOnProjectileStateChanged(OldState, LocalState);

	switch (const auto& OldProjectileState = OldState.GetProjectileState())
	{
	case EProjectileState::Collapsed: break;
	case EProjectileState::Perform:
		StopThrowProjectile();
		PostExitPerformStateReplicated();
		break;
	case EProjectileState::Custom:
		OnReplicatedCustomStateExit(OldState.GetCustomState());
		break;
	default:
		UE_LOG(LogActor, Fatal, TEXT("[%s] Exit from Invalid ProjectileState: %d"), *GetName(), OldProjectileState);
	}

	switch (const auto& NewProjectileState = LocalState.GetProjectileState())
	{
	case EProjectileState::Collapsed: break;
	case EProjectileState::Perform:
		ThrowProjectile(ThrowData);
		break;
	case EProjectileState::Custom:
		OnReplicatedCustomStateEnter(LocalState.GetCustomState());
		break;
	default:
		UE_LOG(LogActor, Fatal, TEXT("[%s] Invalid ProjectileState has replicated: %d"), *GetName(),
		       NewProjectileState);
	}
}

void ALakayaProjectile::CallOnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                const FHitResult& SweepResult)
{
	if (!CollisionComponent->GetMoveIgnoreActors().Contains(OtherActor))
	{
		OnProjectileOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}
