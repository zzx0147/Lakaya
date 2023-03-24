// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponReload.h"
#include "Character/FocusableCharacter.h"
#include "StandardReload.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UStandardReload : public UWeaponReload
{
	GENERATED_BODY()

public:
	UStandardReload();

protected:
	virtual void SetupData(const FName& RowName) override;
	virtual void OnReloadStart() override;
	virtual void OnReloadStartNotify() override;

private:
	void ReloadCallback(const bool& IsSimulated = false);

	UPROPERTY(EditAnywhere)
	class UDataTable* ReloadTable;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AFocusableCharacter> Character;

	UPROPERTY(Replicated)
	TWeakObjectPtr<class UGunComponent> GunComponent;

	UPROPERTY(Replicated)
	float ReloadDelay;

	FTimerHandle ReloadTimer;
	FTimerHandle ClientReloadTimer;
};
