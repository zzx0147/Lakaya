// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageableCharacter.h"
#include "ArmedCharacter.generated.h"

DECLARE_EVENT_OneParam(AArmedCharacter, FWeaponChangeSignature, class UWeaponComponent* const&);

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public ADamageableCharacter
{
	GENERATED_BODY()

public:
	AArmedCharacter(const FObjectInitializer& ObjectInitializer);

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

protected:
	virtual void BeginPlay() override;
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;
	virtual void KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser) override;
	virtual void RespawnNotify_Implementation() override;

public:
	/**
	 * @brief 캐릭터의 첫번째 무기를 설정합니다. 반드시 서버에서 호출해야 합니다.
	 * @param WeaponClassRowName WeaponAssetDataTable에서의 RowName
	 */
	void SetupPrimaryWeapon(const FName& WeaponClassRowName);

	inline class UWeaponComponent* GetPrimaryWeapon() { return PrimaryWeapon; }

	void FireStart();
	void FireStop();
	void AbilityStart();
	void AbilityStop();
	void ReloadStart();
	void ReloadStop();

	// 첫번째 무기가 변경되면 호출됩니다. 물론 초기에 셋업되는 경우에도 호출됩니다.
	FWeaponChangeSignature OnPrimaryWeaponChanged;

private:
	UFUNCTION()
	void OnRep_PrimaryWeapon();
	
	UPROPERTY(EditAnywhere)
	class UDataTable* WeaponClassDataTable;

	UPROPERTY(ReplicatedUsing=OnRep_PrimaryWeapon, VisibleAnywhere)
	class UWeaponComponent* PrimaryWeapon;
};
