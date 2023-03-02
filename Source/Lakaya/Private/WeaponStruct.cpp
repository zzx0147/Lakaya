// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponStruct.h"

#include "WeaponFire.h"
#include "WeaponFireData.h"

void FWeaponStruct::SetupWeaponComponents(AActor* Caller, const FWeaponAssetData& Data, const UDataTable& FireData,
                                          const UDataTable& AbilityData, const UDataTable& ReloadData)
{
	//TODO: 능력, 재장전 컴포넌트를 추가하고, 스태틱 메시도 추가하는 로직을 작성합니다.
	auto FireClass = NewObject<UActorComponent>(Caller, Data.FireClassPath.LoadSynchronous());
	FireComponent = Cast<IWeaponFire>(FireClass);
	auto FireRow = FireData.FindRow<FWeaponFireData>(Data.FireRowName,TEXT("SetupWeapon"));
	if (FireComponent.IsValid()) FireComponent->SetupData(FireRow);
}
