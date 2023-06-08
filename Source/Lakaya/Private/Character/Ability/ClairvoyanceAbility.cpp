// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "ETC/OutlineManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UClairvoyanceAbility::UClairvoyanceAbility() : Super()
{
	AbilityDuration = 10.0f;
	bIsClairvoyanceOn = false;
	SetClairvoyanceState(bIsClairvoyanceOn);
}

void UClairvoyanceAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UClairvoyanceAbility, AbilityStartTime);
}

void UClairvoyanceAbility::OnRep_AbilityStartTime()
{
	// bIsClairvoyanceOn = true;
	SetClairvoyanceState(true);
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
	// if(!AliveState && bIsClairvoyanceOn)
	// {
	// 	AbilityEnd();
	// 	GetWorld()->GetTimerManager().ClearTimer(AbilityStartHandle);
	// }
	if (!AliveState && GetOwner()->HasAuthority())
	{
		AbilityEnd();
		GetWorld()->GetTimerManager().ClearTimer(AbilityStartHandle);
		SetClairvoyanceState(false);
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

void UClairvoyanceAbility::SetClairvoyanceState(const bool& NewState)
{
	if (NewState == bIsClairvoyanceOn) return;
	bIsClairvoyanceOn = NewState;
	OnClairvoyanceChanged.Broadcast(bIsClairvoyanceOn);
}

void UClairvoyanceAbility::AbilityStart()
{
	GetOutlineManager()->RegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	GetWorld()->GetTimerManager().SetTimer(AbilityEndHandle, this, &UClairvoyanceAbility::AbilityEnd,
	                                       AbilityStartTime + AbilityDuration - GetServerTime(), false);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("ClairvoyancecAbilityStart!"));
}

void UClairvoyanceAbility::AbilityEnd()
{
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	GetOutlineManager()->UnRegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	// bIsClairvoyanceOn = false;
	SetClairvoyanceState(false);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("ClairvoyancecAbilityStop!"));
}

void UClairvoyanceAbility::LocalAbilityStart()
{
	Super::LocalAbilityStart();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Client ClairvoyancecAbilityStart!"));
}
