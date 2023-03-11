// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFire.h"
#include "RiffleFire.generated.h"

UENUM(BlueprintType)
enum class EGunSelector : uint8
{
	Semi,
	Burst,
	Auto
};

UCLASS()
class LAKAYA_API URiffleFire : public UWeaponFire
{
	GENERATED_BODY()

public:
	URiffleFire();

protected:
	virtual void SetupData(const FName& RowName) override;
	virtual void OnFireStart() override;
	virtual void OnFireStop() override;
	virtual void OnSwitchSelector() override;
	virtual void OnFireStartNotify() override;
	virtual void OnFireStopNotify() override;
	virtual void OnSwitchSelectorNotify() override;

private:
	inline bool IsOnFiring() { return FireCount != 0; }
	inline bool IsNotFiring() { return FireCount == 0; }

	UFUNCTION(Client, Reliable)
	void EmptyMagazine();

	void TraceFire();
	void StopFire();
	void UpdateFireMode();
	void OnNestedFire();
	void OnFreshFire();

	UPROPERTY(EditAnywhere, Category=DataTable)
	class UDataTable* WeaponFireDataTable;

	// Variables for implementation
	UPROPERTY(Replicated)
	TWeakObjectPtr<class UGunComponent> GunComponent;

	UPROPERTY(Replicated)
	TWeakObjectPtr<class AThirdPersonCharacter> Character;
	
	FCollisionQueryParams TraceQueryParams;
	FTimerHandle FireTimer;
	FTimerHandle SelectorTimer;
	EGunSelector Selector;
	EGunSelector DesiredSelector;
	uint16 FireCount;

	// Loaded data
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
};
