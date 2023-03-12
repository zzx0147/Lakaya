// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFire.h"
#include "RiffleFireServer.generated.h"

UENUM(BlueprintType)
enum class EGunSelector : uint8
{
	Semi,
	Burst,
	Auto
};

UCLASS()
class LAKAYA_API URiffleFireServer : public UWeaponFire
{
	GENERATED_BODY()

public:
	URiffleFireServer();

protected:
	virtual void SetupData(const FName& RowName) override;
	virtual void OnFireStart() override;
	virtual void OnFireStop() override;
	virtual void OnSwitchSelector() override;

	//TODO: 구조를 더 연구해봅시다..
	void FireStartCore(FTimerHandle& GunSelectorTimer,
	                   std::function<bool()> EmptyDeterminant, std::function<void()> OnEmptyMag, std::function<
		                   void()> NestedFire, std::function<void()> FreshFire);

	void FireStopCore(const EGunSelector& GunSelector, uint16& GunFireCount);

	void SwitchSelectorCore(const uint16& GunFireCount, EGunSelector& GunSelector, FTimerHandle& GunSelectorTimer,
	                        std::function<void()> OnUpdateSelector);

	void NestedFireCore(const EGunSelector& GunSelector, uint16& GunFireCount);

	void FreshFireCore(const EGunSelector& GunSelector, uint16& GunFireCount, FTimerHandle& FireTimerHandle,
	                   std::function<void()> FireFunction);

	void FireCallback(uint16& GunFireCount, FTimerHandle& FireTimerHandle, std::function<bool()> EmptyDeterminant, std::function<void()> OnEmpty,
	                  std::function<void()> FireFunction);

	void StopFireCore(uint16& GunFireCount, FTimerHandle& FireTimerHandle);

	inline bool IsOnFiring() { return FireCount != 0; }
	inline bool IsNotFiring() { return FireCount == 0; }

private:
	UFUNCTION(Client, Reliable)
	void EmptyMagazine();

	void TraceFire();
	void UpdateFireMode();

protected:
	UPROPERTY(Replicated)
	TWeakObjectPtr<class UGunComponent> GunComponent;

	UPROPERTY(Replicated)
	TWeakObjectPtr<class AThirdPersonCharacter> Character;

	UPROPERTY(Replicated)
	float BaseDamage;

	UPROPERTY(Replicated)
	float FireDelay;

	UPROPERTY(Replicated)
	float FireRange;

	UPROPERTY(Replicated)
	float SqrFireRange;

	UPROPERTY(Replicated)
	float SwitchingDelay;

private:
	UPROPERTY(EditAnywhere, Category=DataTable)
	class UDataTable* WeaponFireDataTable;

	// Variables for implementation
	FCollisionQueryParams TraceQueryParams;
	FTimerHandle FireTimer;
	FTimerHandle SelectorTimer;
	EGunSelector Selector;
	EGunSelector DesiredSelector;
	uint16 FireCount;
};
