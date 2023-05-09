// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AutoFireAbility.h"
#include "BulletFireAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UBulletFireAbility : public UAutoFireAbility
{
	GENERATED_BODY()

public:
	UBulletFireAbility();

protected:
	virtual void BeginPlay() override;
	virtual bool ShouldFire() override;

	UPROPERTY(EditAnywhere)
	uint8 BulletCost;

private:
	TWeakObjectPtr<class UBulletComponent> BulletComponent;
};
