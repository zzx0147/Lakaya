// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "WeaponAbility.h"
#include "WeaponAssetData.h"
#include "WeaponFire.h"
#include "WeaponReload.h"
#include "Engine/DataTable.h"
#include "UObject/WeakInterfacePtr.h"
#include "WeaponStruct.generated.h"

USTRUCT()
struct FWeaponStruct
{
	GENERATED_USTRUCT_BODY()

public:
	/**
	 * @brief 무기를 구성하는 컴포넌트를 생성하여 액터에 부착하고, 컴포넌트를 셋업합니다.
	 * @param Caller 컴포넌트가 부착될 오브젝트입니다.
	 * @param Data 컴포넌트 클래스에 대한 정보를 담은 데이터입니다.
	 * @param FireData 무기 격발에 관련된 데이터테이블입니다.
	 * @param AbilityData 무기 능력에 관련된 데이터테이블입니다.
	 * @param ReloadData 무기 재장전에 관련된 데이터테이블입니다.
	 */
	void SetupWeaponComponents(AActor* Caller, const FWeaponAssetData& Data, const UDataTable& FireData,
	                           const UDataTable& AbilityData, const UDataTable& ReloadData);

	inline void FireStart()
	{
		if (FireComponent.IsValid()) FireComponent->Invoke(IWeaponFire::Execute_FireStart);
	}

	inline void FireStop()
	{
		if (FireComponent.IsValid()) FireComponent->Invoke(IWeaponFire::Execute_FireStop);
	}

	inline void SwitchSelector()
	{
		if (FireComponent.IsValid()) FireComponent->Invoke(IWeaponFire::Execute_SwitchSelector);
	}

	inline void AbilityStart()
	{
		if (AbilityComponent.IsValid()) AbilityComponent->Invoke(IWeaponAbility::Execute_AbilityStart);
	}

	inline void AbilityStop()
	{
		if (AbilityComponent.IsValid()) AbilityComponent->Invoke(IWeaponAbility::Execute_AbilityStop);
	}

	inline void ReloadStart()
	{
		if (ReloadComponent.IsValid()) ReloadComponent->Invoke(IWeaponReload::Execute_ReloadStart);
	}

	inline void ReloadStop()
	{
		if (ReloadComponent.IsValid()) ReloadComponent->Invoke(IWeaponReload::Execute_ReloadStop);
	}

	TWeakInterfacePtr<IWeaponFire> FireComponent;
	TWeakInterfacePtr<IWeaponAbility> AbilityComponent;
	TWeakInterfacePtr<IWeaponReload> ReloadComponent;
};
