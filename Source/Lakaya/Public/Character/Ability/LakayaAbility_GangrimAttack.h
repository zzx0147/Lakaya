// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/LakayaAbility.h"
#include "LakayaAbility_GangrimAttack.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAbility_GangrimAttack : public ULakayaAbility
{
	GENERATED_BODY()


protected:
	UFUNCTION(BlueprintCallable)
	FHitResult K2_ComponentSweepMulti(UPrimitiveComponent* TargetComp, AActor* TargetActor);
};
