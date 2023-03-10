// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAbility.h"
#include "WeaponFire.h"
#include "WeaponReload.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameStateBase.h"
#include "WeaponComponent.generated.h"


UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void ReadyForReplication() override;

	virtual void RequestSetupData(const FName& RowName);

protected:
	virtual void SetupData();

public:
	inline void FireStart() { if (FireSubObject) FireSubObject->FireStart(); }
	inline void FireStop() { if (FireSubObject) FireSubObject->FireStop(); }
	inline void SwitchSelector() { if (FireSubObject) FireSubObject->SwitchSelector(); }
	inline void AbilityStart() { if (AbilitySubObject) AbilitySubObject->AbilityStart(); }
	inline void AbilityStop() { if (AbilitySubObject) AbilitySubObject->AbilityStop(); }
	inline void ReloadStart() { if (ReloadSubObject) ReloadSubObject->ReloadStart(); }
	inline void ReloadStop() { if (ReloadSubObject) ReloadSubObject->ReloadStop(); }

	//TODO: 비활성화하려는 대상의 형식을 파악하고 비활성화하도록 로직을 수정해야 합니다.
	inline void SetFireEnabled(const bool& Enabled) { if (FireSubObject) FireSubObject->SetEnabled(Enabled); }
	inline void SetAbilityEnabled(const bool& Enabled) { if (AbilitySubObject) AbilitySubObject->SetEnabled(Enabled); }
	inline void SetReloadEnabled(const bool& Enabled) { if (ReloadSubObject) ReloadSubObject->SetEnabled(Enabled); }

private:
	template <class T>
	T* CreateSingleSubObject(UClass* SubObjectClass, const FName& DataRowName);

public:
	UPROPERTY(Replicated, VisibleAnywhere)
	UWeaponFire* FireSubObject;

	UPROPERTY(Replicated, VisibleAnywhere)
	UWeaponAbility* AbilitySubObject;

	UPROPERTY(Replicated, VisibleAnywhere)
	UWeaponReload* ReloadSubObject;

protected:
	FName RequestedRowName;

	UPROPERTY(EditAnywhere)
	class UDataTable* WeaponAssetDataTable;

private:
	bool bIsDataSetupRequested;
};

template <class T>
T* UWeaponComponent::CreateSingleSubObject(UClass* SubObjectClass, const FName& DataRowName)
{
	if (!SubObjectClass) return nullptr;
	T* Ptr = NewObject<T>(this, SubObjectClass);

	if (!Ptr)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Loaded object type error!"));
		return nullptr;
	}

	AddReplicatedSubObject(Ptr);
	Ptr->SetupData(DataRowName);
	return Ptr;
}
