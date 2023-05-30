// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "ETC/OutlineManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UClairvoyanceAbility::UClairvoyanceAbility() : Super()
{
	AbilityDuration = 10.0f;
	bIsClairvoyanceOn = false;
}

void UClairvoyanceAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UClairvoyanceAbility, AbilityStartTime);
}

void UClairvoyanceAbility::OnRep_AbilityStartTime()
{
	bIsClairvoyanceOn = true;
	if (AbilityStartTime > GetServerTime())
		GetWorld()->GetTimerManager().SetTimer(AbilityStartHandle, this, &UClairvoyanceAbility::AbilityStart,AbilityStartTime - GetServerTime(), false);
	else
		AbilityStart();
}

void UClairvoyanceAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Server ClairvoyancecAbilityStart!"));
	AbilityStartTime = RequestTime + AbilityDelay;
	OnRep_AbilityStartTime();
}

void UClairvoyanceAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if(!AliveState && bIsClairvoyanceOn)
	{
		AbilityEnd();
		GetWorld()->GetTimerManager().ClearTimer(AbilityStartHandle);
	}
}

void UClairvoyanceAbility::InitializeComponent()
{
	Super::InitializeComponent();
}

TObjectPtr<AOutlineManager> UClairvoyanceAbility::GetOutlineManager()
{
	if (OutlineManager.IsValid()) return OutlineManager.Get();

	OutlineManager = Cast<AOutlineManager>(UGameplayStatics::GetActorOfClass(this, AOutlineManager::StaticClass()));


	return OutlineManager.Get();
}

bool UClairvoyanceAbility::ShouldStartRemoteCall()
{
	if (bIsClairvoyanceOn) return false;

	return IsEnableTime(GetServerTime() + 0.1f);;
}

void UClairvoyanceAbility::AbilityStart()
{
	GetOutlineManager()->RegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	GetWorld()->GetTimerManager().SetTimer(AbilityEndHandle, this, &UClairvoyanceAbility::AbilityEnd,
	                                       AbilityStartTime + AbilityDuration - GetServerTime(), false);
}

void UClairvoyanceAbility::AbilityEnd()
{
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	GetOutlineManager()->UnRegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	bIsClairvoyanceOn = false;
}

void UClairvoyanceAbility::LocalAbilityStart()
{
	Super::LocalAbilityStart();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Client ClairvoyancecAbilityStart!"));
}
