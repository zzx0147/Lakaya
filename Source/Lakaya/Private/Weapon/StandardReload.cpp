// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/StandardReload.h"

#include "Character/CharAnimInstance.h"
#include "Character/FocusableCharacter.h"
#include "Weapon/GunComponent.h"
#include "Weapon/WeaponReloadData.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

UStandardReload::UStandardReload()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_WeaponReloadDataTable'"));

	if (TableFinder.Succeeded()) ReloadTable = TableFinder.Object;
}

void UStandardReload::SetIsReload_Implementation(bool bIsReload)
{
	UCharAnimInstance* AnimInstance =
		Cast<UCharAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetIsReload(bIsReload);
	}
}

void UStandardReload::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UStandardReload, Character, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UStandardReload, GunComponent, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UStandardReload, ReloadDelay, COND_InitialOnly);
}

void UStandardReload::SetupData(const FName& RowName)
{
	Super::SetupData(RowName);

	Character = Cast<AFocusableCharacter>(FindActor());
	GunComponent = Cast<UGunComponent>(GetOuter());
	if (Character.IsStale()) UE_LOG(LogNetSubObject, Fatal, TEXT("UStandardReload::Character was stale!"));
	if (GunComponent.IsStale()) UE_LOG(LogNetSubObject, Fatal, TEXT("UStandardReload::GunComponent was stale!"));

	auto Data = ReloadTable->FindRow<FWeaponReloadData>(RowName,TEXT("StandardReload"));
	ReloadDelay = Data->ReloadDelay;
}

void UStandardReload::ReloadStart()
{
	ReloadCore(EFocusContext::Owner, [this] { Super::ReloadStart(); });
	SetIsReload(true);
}

void UStandardReload::OnReloadStart()
{
	Super::OnReloadStart();
	ReloadCore(EFocusContext::Server, nullptr, [this] { GunComponent->Reload(); },
	           [this] { UE_LOG(LogNetSubObject, Error, TEXT("Fail to set focus on OnReloadStart!")); });
}

void UStandardReload::OnReloadStartNotify()
{
	Super::OnReloadStartNotify();
	ReloadCore(EFocusContext::Simulated, nullptr, nullptr,
	           [this] { UE_LOG(LogNetSubObject, Error, TEXT("Fail to set focus on OnReloadStartNotify!")); });
}

void UStandardReload::ReloadCore(const EFocusContext& FocusContext, std::function<void()> OnFocus,
                                 std::function<void()> OnRelease, std::function<void()> OnElse)
{
	if (Character->SetFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Reloading))
	{
		if (OnFocus) OnFocus();

		TSharedPtr<FTimerHandle> ReloadTimer = MakeShared<FTimerHandle>();

		GetWorld()->GetTimerManager().SetTimer(*ReloadTimer, [this, FocusContext, OnRelease, ReloadTimer]
		{
			if (Character->ReleaseFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Reloading))
			{
				if (OnRelease) OnRelease();
				GEngine->AddOnScreenDebugMessage(-1, 3.f, GetDebugColor(FocusContext),TEXT("Reload Complete!"));
				SetIsReload(false);
			}
			else
				UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on ReloadCore with %d context!"), FocusContext);
		}, ReloadDelay, false);

		GEngine->AddOnScreenDebugMessage(-1, 3, GetDebugColor(FocusContext),TEXT("Reload Timer Setted!"));
	}
	else if (OnElse) OnElse();
	else UE_LOG(LogNetSubObject, Log, TEXT("Skip ReloadCore!"));
}
