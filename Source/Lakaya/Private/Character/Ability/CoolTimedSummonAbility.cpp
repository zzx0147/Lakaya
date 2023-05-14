// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/CoolTimedSummonAbility.h"

#include "Character/Ability/SummonAbilityInstance.h"
#include "Net/UnrealNetwork.h"

UCoolTimedSummonAbility::UCoolTimedSummonAbility()
{
	bWantsInitializeComponent = true;
	bWantsTransformSet = true;
	ObjectPoolSize = 1;
	CoolTime = 5.f;
	SummonDistance = 50.f;
	SearchFromActor = 5000.f;
}

void UCoolTimedSummonAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCoolTimedSummonAbility, EnableTime);
}

void UCoolTimedSummonAbility::AbilityStart()
{
	// 서버시간 예측의 오차와, 네트워크 딜레이를 감안하여 쿨타임이 돌기 조금 전부터 서버에 능력 사용요청을 할 수는 있도록 합니다.
	if (EnableTime - 0.1f <= GetServerTime()) Super::AbilityStart();
}

void UCoolTimedSummonAbility::InitializeComponent()
{
	Super::InitializeComponent();

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

		AbilityInstance->OnAbilityEnded.AddUObject(this, &UCoolTimedSummonAbility::OnAbilityInstanceEnded);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("AbilityInstance spawned"));
		return AbilityInstance;
	});
}

void UCoolTimedSummonAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (EnableTime > GetServerTime()) return;

	if (const auto AbilityInstance = AbilityInstancePool.GetObject())
	{
		if (bWantsTransformSet)
		{
			const auto Direction = GetNormalToCameraForwardTracePoint(SearchFromActor, CollisionQueryParams);
			AbilityInstance->SetActorLocationAndRotation(GetOwner()->GetActorLocation() + Direction * SummonDistance,
			                                             Direction.Rotation());
		}
		AbilityInstance->OnSummoned();
	}

	EnableTime = GetServerTime() + CoolTime;
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void UCoolTimedSummonAbility::OnRep_EnableTime()
{
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void UCoolTimedSummonAbility::OnAbilityInstanceEnded(ASummonAbilityInstance* const& AbilityInstance)
{
	AbilityInstancePool.ReturnObject(AbilityInstance);
}
