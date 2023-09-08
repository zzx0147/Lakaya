// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Actor/LakayaProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/GameStateBase.h"
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
	SetReplicates(true);

	// PredictProjectilePath가 radius 속성을 갖는 투사체만 지원하므로 투사체는 구체여야 합니다..
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	// 정적 오브젝트에 대해서만 충돌이 가능하도록 설정합니다. 하위 클래스에서 필요한 콜리전 채널에 대해 Overlap을 설정하는 것은 가능합니다.
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileOverlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));

	ProjectileLaunchVelocity = 1000.f;
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
	Key.NewRejectedDelegate().BindUObject(this, &ThisClass::RejectProjectile);

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
	RecentPerformedTime = InThrowData.ServerTime;
	InThrowData.SetupPredictedProjectileParams(PredictedProjectileParams, ProjectileLaunchVelocity,
	                                           GetWorld()->GetGameState()->GetServerWorldTimeSeconds());

	static FPredictProjectilePathResult Result;
	static const auto& LastPoint = Result.LastTraceDestination;
	static TArray<AActor*> IgnoredActors;

	while (UGameplayStatics::PredictProjectilePath(this, PredictedProjectileParams, Result)
		&& PredictedProjectileParams.MaxSimTime > LastPoint.Time)
	{
		const auto& HitResult = Result.HitResult;
		const auto HitResponse = HitResult.GetComponent()->GetCollisionResponseToComponent(CollisionComponent);

		if (HitResponse == ECR_Block)
		{
			if (HitResult.bStartPenetrating)
			{
				//TODO: 벽이나 바닥과 같은 오브젝트에 투사체가 걸쳐져있거나, 그 안에 있습니다. 이 투사체를 꺼내서 다시 투사체를 던지도록 합니다.
			}

			// 투사체는 정적인 오브젝트에 대해서만 Block으로 설정되므로 Block 이벤트는 클라이언트에서 실행되어도 안전합니다.
			OnProjectilePathBlock(Result);

			const auto MirroredVelocity = -LastPoint.Velocity.MirrorByVector(HitResult.ImpactNormal);
			PredictedProjectileParams.LaunchVelocity = MirroredVelocity;
		}
		else
		{
			if (HitResponse == ECR_Overlap && HasAuthority())
			{
				// 오버랩 이벤트는 서버에서만 처리할 수 있도록 합니다.
				OnProjectilePathOverlap(Result);
			}

			// Overlap된 액터는 클라이언트에서는 무시합니다.
			const auto Actor = HitResult.GetActor();
			PredictedProjectileParams.ActorsToIgnore.Emplace(Actor);
			IgnoredActors.Emplace(Actor);

			PredictedProjectileParams.LaunchVelocity = LastPoint.Velocity;
		}

		PredictedProjectileParams.StartLocation = LastPoint.Location;
		PredictedProjectileParams.MaxSimTime -= LastPoint.Time;
	}

	// 무시됐던 액터들을 다시 목록에서 제거합니다.
	for (auto&& IgnoredActor : IgnoredActors)
	{
		PredictedProjectileParams.ActorsToIgnore.RemoveSwap(IgnoredActor);
	}
	IgnoredActors.Reset();

	SetActorLocation(LastPoint.Location);
	if (ProjectileMovementComponent->bRotationFollowsVelocity)
	{
		SetActorRotation(LastPoint.Velocity.Rotation());
	}
	ProjectileMovementComponent->Velocity = LastPoint.Velocity;
}

void ALakayaProjectile::SetCustomState(const uint8& InCustomState)
{
	auto& StateRef = InternalGetProjectileState();
	const auto OldState = StateRef;
	if (StateRef.SetCustomState(InCustomState))
	{
		BroadcastOnProjectileStateChanged(OldState, StateRef);
	}
}

void ALakayaProjectile::OnRep_CustomState()
{
	UE_LOG(LogActor, Log, TEXT("[%s] Custom state replicated : %d"), *GetName(), LocalState.GetCustomState());
}

void ALakayaProjectile::OnProjectilePathOverlap_Implementation(const FPredictProjectilePathResult& PredictResult)
{
}

void ALakayaProjectile::OnProjectilePathBlock_Implementation(const FPredictProjectilePathResult& PredictResult)
{
}

void ALakayaProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 오버랩 이벤트를 수신할 수 있도록 합니다.
	CollisionComponent->SetGenerateOverlapEvents(HasAuthority());
}

void ALakayaProjectile::OnProjectileOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                           bool bFromSweep, const FHitResult& SweepResult)
{
}

void ALakayaProjectile::OnCollapsed_Implementation()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//TODO
}

void ALakayaProjectile::SetProjectileState(const EProjectileState& InProjectileState)
{
	auto& StateRef = InternalGetProjectileState();
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
	RecentPerformedTime = ThrowData.ServerTime;
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
