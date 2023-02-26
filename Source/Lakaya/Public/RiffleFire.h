// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFire.h"
#include "Components/ActorComponent.h"
#include "RiffleFire.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API URiffleFire : public UActorComponent, public IWeaponFire
{
	GENERATED_BODY()

public:
	void SwitchFireMode();

private:
	void FireStart_Implementation(const float& Time);
	void FireStop_Implementation(const float& Time);

	void TraceFire();
};
