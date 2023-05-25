// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/CoolTimedSummonAbility.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Ability/SummonAbilityInstance.h"
#include "Components/ArrowComponent.h"

UCoolTimedSummonAbility::UCoolTimedSummonAbility()
{
	bWantsInitializeComponent = true;
	bCanEverStartRemoteCall = true;
	ObjectPoolSize = 1;
	BaseCoolTime = 5.f;
	SummonDistance = 50.f;
	SearchFromActor = 5000.f;
}

void UCoolTimedSummonAbility::InitializeComponent()
{
	Super::InitializeComponent();

	// 총구 컴포넌트 탐색
	if (auto MuzzleComponents = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), FName("Muzzle"));
		MuzzleComponents.IsValidIndex(0))
		MuzzleComponent = Cast<UArrowComponent>(MuzzleComponents[0]);

	if (MuzzleNiagaraSystem)
	{
		MuzzleNiagara =
			UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleNiagaraSystem, MuzzleComponent.Get(), NAME_None,
			                                             FVector::ZeroVector, FRotator::ZeroRotator,
			                                             EAttachLocation::SnapToTarget, false, false);
	}

	// 서버에서는 오브젝트풀을 세팅합니다.
	if (!GetOwner()->HasAuthority()) return;
	AbilityInstancePool.SetupObjectPool(ObjectPoolSize, [this]()-> ASummonAbilityInstance* {
		FActorSpawnParameters Params;
		Params.Instigator = GetOwner<APawn>();
		Params.Owner = Params.Instigator ? Params.Instigator->GetController() : nullptr;
		const auto AbilityInstance = GetWorld()->SpawnActor<ASummonAbilityInstance>(AbilityInstanceClass, Params);

		if (!AbilityInstance)
		{
			UE_LOG(LogScript, Error, TEXT("Fail to spawn projectile!"));
			return nullptr;
		}

		AbilityInstance->SetOwningAbility(this);
		AbilityInstance->SetTeam(GetPlayerTeam());
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("AbilityInstance spawned"));
		return AbilityInstance;
	});
}

bool UCoolTimedSummonAbility::ShouldStartRemoteCall()
{
	// 서버시간 예측의 오차와, 네트워크 딜레이를 감안하여 쿨타임이 돌기 조금 전부터 서버에 능력 사용요청을 할 수는 있도록 합니다.
	return IsEnableTime(GetServerTime() + 0.1f);
}

void UCoolTimedSummonAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);
	if (!IsEnableTime(GetServerTime()) || !CostResource(ResourceCost)) return;

	if (const auto AbilityInstance = AbilityInstancePool.GetObject())
		AbilityInstance->SetAbilityInstanceState(EAbilityInstanceState::Ready);

	ApplyCoolTime();
}

void UCoolTimedSummonAbility::GetSummonLocationAndRotation(FVector& Location, FRotator& Rotator) const
{
	const auto Direction = GetNormalToCameraForwardTracePoint(SearchFromActor, CollisionQueryParams);
	Location = GetOwner()->GetActorLocation() + Direction * SummonDistance;
	Rotator = Direction.Rotation();
}

void UCoolTimedSummonAbility::NotifyPerformTime(const float& Time)
{
	OnPerformTimeNotified.Broadcast(Time);
}

void UCoolTimedSummonAbility::NotifyAbilityInstanceCollapsed(ASummonAbilityInstance* const& AbilityInstance)
{
	AbilityInstancePool.ReturnObject(AbilityInstance);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object returned!"));
}

void UCoolTimedSummonAbility::SetTeam(const EPlayerTeam& Team)
{
	Super::SetTeam(Team);
	AbilityInstancePool.ForEach([Team](ASummonAbilityInstance* Instance) { Instance->SetTeam(Team); });
}
