// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/DeathRayAbility.h"

#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"

UDeathRayAbility::UDeathRayAbility()
{
	AbilityDuration = 8.0f;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UDeathRayAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);


	float RemainDelay = AbilityStartTime - GetServerTime();
	if (RemainDelay < 0.0f) RemainDelay = 0.0f;
	
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("DeathRayRemoteStart"));
	GetWorld()->GetTimerManager().SetTimer(AbilityTimer, [this]
	{
		RemoteAbilityStop(GetServerTime());
	}, RemainDelay + AbilityDuration, false);
}

void UDeathRayAbility::LocalAbilityStart()
{
	Super::LocalAbilityStart();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("DeathRayLocalStart"));
}

bool UDeathRayAbility::ShouldStartRemoteCall()
{
	if(IsFiring()) return false;
	return IsEnableTime(GetServerTime() + 0.1f);
}

void UDeathRayAbility::RemoteAbilityStop(const float& RequestTime)
{
	Super::RemoteAbilityStop(RequestTime);
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
}

void UDeathRayAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(!IsActive()) return;

	FVector EndPoint = GetCameraForwardTracePoint(5000.0f, CollisionQueryParams);

	if (LaserEffect.IsValid() && MuzzleComponent.IsValid())
	{
		//if(GetOwner()->HasAuthority()) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, *FString::Printf(TEXT("%f"),AbilityStartTime));
		FVector result = MuzzleComponent->GetComponentTransform().InverseTransformPosition(EndPoint);
		LaserEffect->SetNiagaraVariableVec3(TEXT("BeamEnd"), result);
	}
}

void UDeathRayAbility::OnRep_AbilityStartTime()
{
	Super::OnRep_AbilityStartTime();

	if(!LaserEffect.IsValid()) return;
	
	OnDeathRayPerformTimeNotified.Broadcast(AbilityStartTime);

	if(AbilityStartTime < 0.0f) LaserEffect->Deactivate();//AbilityStartTime이 음수면 스킬이 Off이므로 Deactivate

	if(AbilityStartTime > GetServerTime())//AbilityStartTime이 현재 시간보다 뒤면 해당 시간까지 기다렸다가 Activate
	{
		FTimerHandle timer;
		GetWorld()->GetTimerManager().SetTimer(timer,[this]{LaserEffect->Activate();},AbilityStartTime - GetServerTime(),false);
		return;
	}

	if (AbilityStartTime + AbilityDuration < GetServerTime()) return; //만약 AbilityStartTime + AbilityDuration보다 현재 시간이 더 크다면 능력이 이미 끝난것이므로 return
	
	LaserEffect->Activate();//AbilityStartTime이 이미 지났다면 바로 시작
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
