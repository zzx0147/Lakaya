// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/CoolTimedSummonAbility.h"

#include "Character/Ability/SummonAbilityInstance.h"

UCoolTimedSummonAbility::UCoolTimedSummonAbility()
{
	bWantsInitializeComponent = true;
	bCanEverStartRemoteCall = true;
	ObjectPoolSize = 1;
	BaseCoolTime = 5.f;
	SummonDistance = 50.f;
	SearchFromActor = 5000.f;
	SetIsReplicatedByDefault(true);
}

void UCoolTimedSummonAbility::InitializeComponent()
{
	Super::InitializeComponent();

	// 서버에서는 오브젝트풀을 세팅합니다.
	if (!GetOwner()->HasAuthority()) return;
	FActorSpawnParameters Params;
	Params.Instigator = GetOwner<APawn>();
	Params.Owner = Params.Instigator ? Params.Instigator->GetController() : nullptr;
	AbilityInstancePool.SetupObjectPool(ObjectPoolSize, [this, Params]()-> ASummonAbilityInstance* {
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
}

void UCoolTimedSummonAbility::GetSummonLocationAndRotation(FVector& Location, FRotator& Rotator) const
{
	const auto Direction = GetNormalToCameraForwardTracePoint(SearchFromActor, CollisionQueryParams,
	                                                          BasisComponent.Get());
	Location = (BasisComponent ? BasisComponent->GetComponentLocation() : GetOwner()->GetActorLocation()) +
		Direction * SummonDistance;
	Rotator = Direction.Rotation();
}

void UCoolTimedSummonAbility::OnAbilityInstanceStateChanged(const EAbilityInstanceState& InstanceState,
                                                            ASummonAbilityInstance* const& AbilityInstance)
{
	if (!AbilityInstance->HasAuthority()) return;
	switch (InstanceState)
	{
	case EAbilityInstanceState::Collapsed:
		AbilityInstancePool.ReturnObject(AbilityInstance);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object returned!"));
		break;
	case EAbilityInstanceState::Perform:
		ApplyCoolTime();
		break;
	default: break;
	}
}

void UCoolTimedSummonAbility::SetBasisComponent(USceneComponent* NewComponent)
{
	BasisComponent = NewComponent;
}

void UCoolTimedSummonAbility::SetTeam(const EPlayerTeam& Team)
{
	Super::SetTeam(Team);
	AbilityInstancePool.ForEach([Team](ASummonAbilityInstance* Instance) { if (Instance) Instance->SetTeam(Team); });
}
