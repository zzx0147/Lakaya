// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponStruct.h"

#include "WeaponFire.h"
#include "WeaponFireData.h"

void FWeaponStruct::SetupWeaponComponents(AActor* Caller, const FWeaponAssetData& Data)
{
	SetupComponent(Caller, Data.FireClassPath.LoadSynchronous(), FireComponent, Data.FireRowName);
	SetupComponent(Caller, Data.AbilityClassPath.LoadSynchronous(), AbilityComponent, Data.AbilityRowName);
	SetupComponent(Caller, Data.ReloadClassPath.LoadSynchronous(), ReloadComponent, Data.ReloadRowName);
	//TODO: 총기의 스태틱 메시를 추가하는 로직을 추가합니다.
}
