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
	virtual void UpgradeWeapon();
	virtual void UpgradeInitialize();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void SetupData();
	virtual void SetupUI();


	UFUNCTION()
	virtual void OnRep_UpgradeLevel();

public:
	inline void FireStart() { if (FireSubObject) FireSubObject->FireStart(); }
	inline void FireStop() { if (FireSubObject) FireSubObject->FireStop(); }
	inline void AbilityStart() { if (AbilitySubObject) AbilitySubObject->AbilityStart(); }
	inline void AbilityStop() { if (AbilitySubObject) AbilitySubObject->AbilityStop(); }
	inline void ReloadStart() { if (ReloadSubObject) ReloadSubObject->ReloadStart(); }
	inline void ReloadStop() { if (ReloadSubObject) ReloadSubObject->ReloadStop(); }

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

	UPROPERTY(EditAnywhere)
	class UDataTable* WeaponUpgradeDataTable;

	UPROPERTY(ReplicatedUsing = OnRep_UpgradeLevel)
	int8 UpgradeLevel;

	class UGamePlayConsecutiveKillsWidget* ConsecutiveKillsWidget;
	class UGamePlayBulletWidget* BulletWidget;

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

	Ptr->SetupData(DataRowName);
	AddReplicatedSubObject(Ptr);
	return Ptr;
}
