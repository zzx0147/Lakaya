// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiffleFireCore.h"
#include "RiffleFireServer.generated.h"

UCLASS(Abstract)
class LAKAYA_API URiffleFireServer : public URiffleFireCore
{
	GENERATED_BODY()

public:
	URiffleFireServer();

protected:
	virtual void SetupData(const FName& RowName) override;
	virtual void OnFireStart() override;
	virtual void OnFireStop() override;
	virtual void OnSwitchSelector() override;

private:
	UFUNCTION(Client, Reliable)
	void EmptyMagazine();
	
	void TraceFire();

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
