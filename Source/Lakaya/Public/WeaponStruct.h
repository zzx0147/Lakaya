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

//TODO: UWeaponComponent를 하나 만들고, 그 액터 컴포넌트의 서브오브젝트로 Fire, Reload, Ability의 역할을 수행하는 클래스를 생성하도록 변경합니다.
USTRUCT()
struct FWeaponStruct
{
	GENERATED_USTRUCT_BODY()

public:
	/**
	 * @brief 무기를 구성하는 컴포넌트를 생성하여 액터에 부착하고, 컴포넌트를 셋업합니다.
	 * @param Caller 컴포넌트가 부착될 오브젝트입니다.
	 * @param Data 컴포넌트 클래스에 대한 정보를 담은 데이터입니다.
	 */
	void SetupWeaponComponents(AActor* Caller, const FWeaponAssetData& Data);

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

private:
	template <class T>
	void SetupComponent(AActor* Caller, UClass* ComponentClass, TWeakInterfacePtr<T>& Ptr,
	                    const FName& DataRowName);

public:
	TWeakInterfacePtr<IWeaponFire> FireComponent;
	TWeakInterfacePtr<IWeaponAbility> AbilityComponent;
	TWeakInterfacePtr<IWeaponReload> ReloadComponent;
};

template <class T>
void FWeaponStruct::SetupComponent(AActor* Caller, UClass* ComponentClass, TWeakInterfacePtr<T>& Ptr,
                                   const FName& DataRowName)
{
	auto Component = Caller->AddComponentByClass(TSubclassOf<UActorComponent>(ComponentClass), false,
	                                             FTransform::Identity, false);
	if (!Component) return;

	Ptr = Cast<T>(Component);
	if (!Ptr.IsValid())
	{
		Component->DestroyComponent();
		UE_LOG(LogActor, Error, TEXT("Loaded actor component was not subclass of IWeaponBase"));
		return;
	}

	IWeaponBase::Execute_SetupData(Ptr.GetObject(), DataRowName);
}
