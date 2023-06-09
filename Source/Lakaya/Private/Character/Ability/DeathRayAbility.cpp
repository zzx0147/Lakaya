// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/DeathRayAbility.h"

#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"

UDeathRayAbility::UDeathRayAbility()
{
	DelayedAbilityDuration = 8.0f;
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UDeathRayAbility::LocalAbilityStart()
{
	Super::LocalAbilityStart();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("DeathRayLocalStart"));
}

bool UDeathRayAbility::ShouldStartRemoteCall()
{
	if(IsFiring()) return false;
	if(IsDelayedAbilityRunning()) return false;
	return IsEnableTime(GetServerTime() + 0.1f);
}

void UDeathRayAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(!IsActive()) return;

	const FVector EndPoint = GetCameraForwardTracePoint(5000.0f, CollisionQueryParams);

	if (LaserEffect.IsValid() && MuzzleComponent.IsValid())
	{
		//if(GetOwner()->HasAuthority()) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, *FString::Printf(TEXT("%f"),AbilityStartTime));
		const FVector Result = MuzzleComponent->GetComponentTransform().InverseTransformPosition(EndPoint);
		LaserEffect->SetNiagaraVariableVec3(TEXT("BeamEnd"), Result);
	}
}

void UDeathRayAbility::OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime)
{
	Super::OnDelayedAbilityStartTimeChanged(NewDelayedAbilityStartTime);
	if(!LaserEffect.IsValid()) return;
	
	OnDeathRayPerformTimeNotified.Broadcast(NewDelayedAbilityStartTime);

	if(NewDelayedAbilityStartTime < 0.0f) LaserEffect->Deactivate();//AbilityStartTime이 음수면 스킬이 Off이므로 Deactivate
}

void UDeathRayAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	if(LaserEffect.IsValid()) LaserEffect->Activate();
}

void UDeathRayAbility::StopDelayedAbility()
{
	Super::StopDelayedAbility();
	if(LaserEffect.IsValid()) LaserEffect->Deactivate();
}

void UDeathRayAbility::InitializeComponent()
{
	Super::InitializeComponent();
	if (auto NiagaraComponents = GetOwner()->GetComponentsByTag(UNiagaraComponent::StaticClass(), FName("LaserEffect"));
		NiagaraComponents.IsValidIndex(0))
		LaserEffect = Cast<UNiagaraComponent>(NiagaraComponents[0]);

	if (auto MuzzleComponents = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), FName("Muzzle"));
	MuzzleComponents.IsValidIndex(0))
		MuzzleComponent = Cast<UArrowComponent>(MuzzleComponents[0]);
}
