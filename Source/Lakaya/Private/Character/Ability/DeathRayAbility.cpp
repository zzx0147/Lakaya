// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/DeathRayAbility.h"

#include "NiagaraComponent.h"

UDeathRayAbility::UDeathRayAbility()
{
	bCanEverStopRemoteCall = false;
	BaseCoolTime = 5.f;
	BaseAbilityDuration = 8.f;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UDeathRayAbility::ShouldStartRemoteCall()
{
	return !IsWantsToFire() && IsEnableTime(GetServerTime() + 0.1f);
}

void UDeathRayAbility::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!LaserEffect.IsValid() || !BasisComponent) return;
	const FVector EndPoint = GetCameraForwardTracePoint(FireRange, CollisionQueryParams);
	const FVector Result = BasisComponent->GetComponentTransform().InverseTransformPosition(EndPoint);
	LaserEffect->SetNiagaraVariableVec3(TEXT("BeamEnd"), Result);
}

void UDeathRayAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(DeathRayTimer, this, &UDeathRayAbility::DeathRayTimerCallback,
		                                       BaseAbilityDuration);
	}
	if (!LaserEffect.IsValid()) return;
	LaserEffect->Activate();
	SetComponentTickEnabled(true);
}

void UDeathRayAbility::StopDelayedAbility()
{
	Super::StopDelayedAbility();
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	if (LaserEffect.IsValid()) LaserEffect->DeactivateImmediate();
}

void UDeathRayAbility::OnDelayedAbilityStopTimeChanged(const float& NewDelayedAbilityStopTime)
{
	Super::OnDelayedAbilityStopTimeChanged(NewDelayedAbilityStopTime);
	if (!LaserEffect.IsValid()) return;
	LaserEffect->Deactivate();
	SetComponentTickEnabled(false);
}

void UDeathRayAbility::DeathRayTimerCallback()
{
	RemoteAbilityStop(GetServerTime());
}

void UDeathRayAbility::InitializeComponent()
{
	Super::InitializeComponent();
	if (auto NiagaraComponents = GetOwner()->GetComponentsByTag(UNiagaraComponent::StaticClass(), FName("LaserEffect"));
		NiagaraComponents.IsValidIndex(0))
		LaserEffect = Cast<UNiagaraComponent>(NiagaraComponents[0]);
}
