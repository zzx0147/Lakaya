// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockstep.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UCLASS(Config=Game, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	DEFINE_LOCKSTEP_NOTIFY(FireStart, NetMulticast);
	DEFINE_LOCKSTEP_NOTIFY(FireStop, NetMulticast);
	DEFINE_LOCKSTEP(SwitchSelector);

protected:
	UPROPERTY(Config)
	float LockstepDelay = 0.1f;
};
