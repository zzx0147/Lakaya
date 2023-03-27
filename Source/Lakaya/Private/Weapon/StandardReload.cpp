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
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponReloadDataTable'"));

	if (TableFinder.Succeeded()) ReloadTable = TableFinder.Object;
}

void UStandardReload::SetIsReload_Implementation(bool bIsReload)
{
	UCharAnimInstance* AnimInstance = Cast<UCharAnimInstance>(Character->GetMesh()->GetAnimInstance());
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

void UStandardReload::OnReloadStart()
{
	Super::OnReloadStart();

	UE_LOG(LogNetSubObject, Log, TEXT("OnReloadStart"));

	SetIsReload(true);

	if (!Character->SetFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Reloading))
	{
		UE_LOG(LogNetSubObject, Log, TEXT("It was not focusable"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimer,
	                                       [this]
	                                       {
		                                       GunComponent->Reload();
		                                       ReloadCallback();
	                                       },
	                                       ReloadDelay - LockstepDelay, false);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Reload Started"));
}

void UStandardReload::OnReloadStartNotify()
{
	Super::OnReloadStartNotify();

	if (!Character->SetFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Reloading))
	{
		UE_LOG(LogNetSubObject, Log, TEXT("Reload ignored in OnReloadStartNotify"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ClientReloadTimer,
	                                       [this] { ReloadCallback(true); },
	                                       ReloadDelay - LockstepDelay, false);
}

void UStandardReload::ReloadCallback(const bool& IsSimulated)
{
	UE_LOG(LogNetSubObject, Log, TEXT("ReloadCallback"));
	Character->ReleaseFocus(IsSimulated ? EFocusContext::Simulated : EFocusContext::Server, EFocusSpace::MainHand,
	                        EFocusState::Reloading);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, IsSimulated ? FColor::Red : FColor::White,
	                                 TEXT("Reload Complete"));
	SetIsReload(false);
}
