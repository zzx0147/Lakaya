// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFire.h"
#include "RiffleFire.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
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

private:
	virtual void SetupData_Implementation(const FName& RowName) override;
	virtual void FireStart_Implementation(const float& Time) override;
	virtual void FireStop_Implementation(const float& Time) override;
	virtual void SwitchSelector_Implementation(const float& Time) override;
	virtual void FireStartNotify_Implementation(const float& Time) override;
	virtual void FireStopNotify_Implementation(const float& Time) override;
	virtual void SwitchSelectorNotify_Implementation(const float& Time) override;

	float LockstepTimerTime(const float& Time) const;
	void TraceFire();
	void StopFire();
	void UpdateFireMode();

	UPROPERTY(EditAnywhere, Category=DataTable)
	class UDataTable* WeaponFireDataTable;

	EFireMode FireMode;
	EFireMode DesiredFireMode;
	uint8 FireCount;
	FTimerHandle StartTimer;
	FTimerHandle StopTimer;
	FTimerHandle SelectorTimer;
	float LastStartTime;
	float LastStopTime;
	float LastSelectorTime;
	TWeakObjectPtr<class AThirdPersonCharacter> Character;
	FCollisionQueryParams TraceQueryParams;

	float BaseDamage;
	float FireDelay;
	float FireRange;
	float SwitchingDelay;
};
