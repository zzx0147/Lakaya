// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "Blueprint/UserWidget.h"
#include "ETC/OutlineManager.h"
#include "Kismet/GameplayStatics.h"

UClairvoyanceAbility::UClairvoyanceAbility() : Super()
{
	bCanEverStartRemoteCall = bUpdateStartTimeOnStart = true;
	AbilityStartDelay = 0.2f;
	BaseAbilityDuration = 30.f;
	BaseCoolTime = 5.f;
	
}

void UClairvoyanceAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !AliveState && TimerManager.TimerExists(ClairvoyanceTimer))
	{
		TimerManager.ClearTimer(ClairvoyanceTimer);
		DisableClairvoyance();
	}
}

void UClairvoyanceAbility::SetEffectMaterial(UMaterialInstanceDynamic* NewEffectMaterial)
{
	EffectMaterial = NewEffectMaterial;
}

void UClairvoyanceAbility::CreateClairvoyanceWidget(APlayerController* PlayerController)
{
	if(PlayerController == nullptr) return;
	if(ClairvoyanceWidgetClass == nullptr) return;
	
	ClairvoyanceWidget = CreateWidget<UUserWidget>(PlayerController, ClairvoyanceWidgetClass);
	if(ClairvoyanceWidget == nullptr) return;
	ClairvoyanceWidget->AddToViewport();
	ClairvoyanceWidget->SetVisibility(ESlateVisibility::Hidden);
}

TObjectPtr<AOutlineManager> UClairvoyanceAbility::GetOutlineManager()
{
	if (OutlineManager.IsValid()) return OutlineManager.Get();
	OutlineManager = Cast<AOutlineManager>(UGameplayStatics::GetActorOfClass(this, AOutlineManager::StaticClass()));
	return OutlineManager.Get();
}

void UClairvoyanceAbility::DisableClairvoyance()
{
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	GetOutlineManager()->UnRegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue(TEXT("ClairvoyanceEffectOpacity"), 0.0f);
	if (ClairvoyanceWidget.IsValid()) ClairvoyanceWidget->SetVisibility(ESlateVisibility::Hidden);
}

bool UClairvoyanceAbility::ShouldStartRemoteCall()
{
	return !GetWorld()->GetTimerManager().TimerExists(ClairvoyanceTimer) && IsEnableTime(GetServerTime() + 0.1f);
}

void UClairvoyanceAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	GetOutlineManager()->RegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue(TEXT("ClairvoyanceEffectOpacity"), 0.2f);
	if (ClairvoyanceWidget.IsValid()) ClairvoyanceWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(ClairvoyanceTimer, this, &UClairvoyanceAbility::DisableClairvoyance,
	                                       BaseAbilityDuration);
}
