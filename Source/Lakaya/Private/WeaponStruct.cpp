// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponStruct.h"

#include "WeaponFire.h"
#include "WeaponFireData.h"

void FWeaponStruct::SetupWeaponComponents(AActor* Caller, const FWeaponAssetData& Data)
{
	//TODO: 능력, 재장전 컴포넌트를 추가하고, 스태틱 메시도 추가하는 로직을 작성합니다.
	auto FireClass = NewObject<UActorComponent>(Caller, Data.FireClassPath.LoadSynchronous());
	FireComponent = Cast<IWeaponFire>(FireClass);
	if (FireComponent.IsValid())
	{
		Caller->FinishAndRegisterComponent(FireClass);
		IWeaponFire::Execute_SetupData(FireComponent.GetObject(), Data.FireRowName);
	}
	else
	{
		FireClass->DestroyComponent();
		UE_LOG(LogActor, Error, TEXT("Loaded actor component was not IWeaponFire."));
	}
}
