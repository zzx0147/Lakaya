// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "Blueprint/UserWidget.h"
#include "Character/LakayaBaseCharacter.h"
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
	if (PlayerController == nullptr) return;
	if (ClairvoyanceWidgetClass == nullptr) return;

	ClairvoyanceWidget = CreateWidget<UUserWidget>(PlayerController, ClairvoyanceWidgetClass);
	if (ClairvoyanceWidget == nullptr) return;
	ClairvoyanceWidget->AddToViewport();
	ClairvoyanceWidget->SetVisibility(ESlateVisibility::Hidden);
}

TObjectPtr<AOutlineManager> UClairvoyanceAbility::GetOutlineManager()
{
	if (CachedOutlineManager.IsValid()) return CachedOutlineManager.Get();
	CachedOutlineManager = Cast<AOutlineManager>(
		UGameplayStatics::GetActorOfClass(this, AOutlineManager::StaticClass()));
	return CachedOutlineManager.Get();
}

void UClairvoyanceAbility::DisableClairvoyance()
{
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	if (const auto OutlineManager = GetOutlineManager(); ensure(OutlineManager))
	{
		OutlineManager->UnRegisterClairvoyance(GetOwner<ALakayaBaseCharacter>());
	}
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
	if (const auto OutlineManager = GetOutlineManager(); ensure(OutlineManager))
	{
		OutlineManager->RegisterClairvoyance(GetOwner<ALakayaBaseCharacter>());
	}
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue(TEXT("ClairvoyanceEffectOpacity"), 0.2f);
	if (ClairvoyanceWidget.IsValid()) ClairvoyanceWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(ClairvoyanceTimer, this, &UClairvoyanceAbility::DisableClairvoyance,
	                                       BaseAbilityDuration);
}
