// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/DeathRayAbility.h"

#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"

UDeathRayAbility::UDeathRayAbility()
{
	AbilityDuration = 8.0f;
	OnFiringStateChanged.AddUObject(this, &UDeathRayAbility::OnChangeIsFiring);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UDeathRayAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("DeathRayRemoteStart"));
	GetWorld()->GetTimerManager().SetTimer(AbilityTimer, [this]
	{
		RemoteAbilityStop(GetServerTime());
	}, AbilityDuration, false);
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

void UDeathRayAbility::OnChangeIsFiring(bool bIsFiringArg)
{
	if (LaserEffect.IsValid()) bIsFiringArg ? LaserEffect->Activate() : LaserEffect->Deactivate();
}

void UDeathRayAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsFiring()) return;

	FVector EndPoint = GetCameraForwardTracePoint(5000.0f, CollisionQueryParams);

	if (LaserEffect.IsValid() && MuzzleComponent.IsValid())
	{
		FVector result = MuzzleComponent->GetComponentTransform().InverseTransformPosition(EndPoint);
		LaserEffect->SetNiagaraVariableVec3(TEXT("BeamEnd"), result);
	}
}

void UDeathRayAbility::InitializeComponent()
{
	Super::InitializeComponent();
	if (auto NiagaraComponents = GetOwner()->GetComponentsByTag(UNiagaraComponent::StaticClass(), FName("LazerEffect"));
		NiagaraComponents.IsValidIndex(0))
		LaserEffect = Cast<UNiagaraComponent>(NiagaraComponents[0]);

	if (auto MuzzleComponents = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), FName("Muzzle"));
	MuzzleComponents.IsValidIndex(0))
		MuzzleComponent = Cast<UArrowComponent>(MuzzleComponents[0]);
}
