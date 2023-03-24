// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "GunComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UGunComponent : public UWeaponComponent
{
	GENERATED_BODY()

public:
	UGunComponent();
	
	inline const uint16& GetMagazineCapacity() const { return MagazineCapacity; }
	inline const uint16& GetRemainBullets() const { return RemainBullets; }

	/**
	 * @brief RemainBullets를 채워넣습니다. 수치 변경 이외의 다른 작업은 수행하지 않습니다.
	 */
	virtual void Reload();

	/**
	 * @brief RemainBullets가 일정량만 채워집니다. 마찬가지로 수치 변경 이외의 다른 작업은 수행하지 않습니다.
	 * @param Bullets 채워질 탄약 수입니다.
	 */
	virtual void Reload(const uint16& Bullets);

	/**
	 * @brief RemainBullets를 차감합니다.
	 * @param Bullets 차감할 양입니다.
	 * @return 차감이 처리되었는지 여부입니다.
	 */
	virtual bool CostBullets(const uint16& Bullets);

protected:
	virtual void SetupData() override;

	UFUNCTION()
	virtual void OnRep_MagazineCapacity() { return; }

	UFUNCTION()
	virtual void OnRep_RemainBullets() { return; }

private:
	UPROPERTY(ReplicatedUsing = OnRep_MagazineCapacity)
	uint16 MagazineCapacity;

	UPROPERTY(ReplicatedUsing = OnRep_RemainBullets)
	uint16 RemainBullets;
};
