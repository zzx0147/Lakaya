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

	inline void FireStart()
	{
		if (FireSubObject)
			FireSubObject->FireStart(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	inline void FireStop()
	{
		if (FireSubObject)
			FireSubObject->FireStop(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	inline void SwitchSelector()
	{
		if (FireSubObject)
			FireSubObject->SwitchSelector(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	inline void AbilityStart()
	{
		if (AbilitySubObject)
			AbilitySubObject->AbilityStart(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	inline void AbilityStop()
	{
		if (AbilitySubObject)
			AbilitySubObject->AbilityStop(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	inline void ReloadStart()
	{
		if (ReloadSubObject)
			ReloadSubObject->ReloadStart(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

	inline void ReloadStop()
	{
		if (ReloadSubObject)
			ReloadSubObject->ReloadStop(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}

private:
	template <class T>
	T* CreateSingleSubObject(UClass* SubObjectClass, const FName& DataRowName);

public:
	UPROPERTY(Replicated)
	UWeaponFire* FireSubObject;

	UPROPERTY(Replicated)
	UWeaponAbility* AbilitySubObject;

	UPROPERTY(Replicated)
	UWeaponReload* ReloadSubObject;

private:
	UPROPERTY(EditAnywhere)
	class UDataTable* WeaponAssetDataTable;

	FName RequestedRowName;
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
