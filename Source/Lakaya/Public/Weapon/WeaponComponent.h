// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAbility.h"
#include "WeaponFire.h"
#include "WeaponReload.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

DECLARE_EVENT_OneParam(UWeaponComponent, FUpgradeLevelSignature, const uint8&)

UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void ReadyForReplication() override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void RequestSetupData(const FName& RowName);
	virtual void UpgradeWeapon();
	virtual void UpgradeInitialize();
	const uint8& GetMaximumUpgradeLevel() const { return MaximumUpgradeLevel; }
	const uint8& GetUpgradeLevel() const { return UpgradeLevel; }

protected:
	virtual void SetupData();

	UFUNCTION()
	virtual void OnRep_UpgradeLevel();

public:
	void FireStart();
	void FireStop();
	void AbilityStart();
	void AbilityStop();
	void ReloadStart();
	void ReloadStop();

	// 캐릭터가 사망시 호출됩니다. 이 함수는 NetMulticast와 같이 모든 인스턴스에서 호출됩니다.
	virtual void OnCharacterDead();

	// 캐릭터가 부활하고 나서 호출됩니다. 이 함수는 모든 인스턴스에서 호출됩니다.
	virtual void OnCharacterRespawn();

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

	// 무기 업그레이드 단계가 변경되면 호출되는 이벤트입니다.
	FUpgradeLevelSignature OnUpgradeLevelChanged;

protected:
	FName RequestedRowName;

	UPROPERTY(EditAnywhere)
	class UDataTable* WeaponAssetDataTable;

	UPROPERTY(EditAnywhere)
	UDataTable* WeaponUpgradeDataTable;

	UPROPERTY(EditAnywhere)
	uint8 MaximumUpgradeLevel;

	UPROPERTY(ReplicatedUsing = OnRep_UpgradeLevel)
	uint8 UpgradeLevel;

private:
	bool bIsDataSetupRequested;
	bool bIsDead;
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

	Ptr->SetupData(DataRowName);
	AddReplicatedSubObject(Ptr);
	return Ptr;
}
