// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_GangrimAttack.h"

FHitResult ULakayaAbility_GangrimAttack::K2_ComponentSweepMulti(UPrimitiveComponent* TargetComp, AActor* TargetActor)
{
	TArray<FHitResult> Results;
	FComponentQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
	
	GetWorld()->ComponentSweepMulti(Results, TargetComp, TargetComp->GetComponentLocation() + FVector(-0.1f),
	                                TargetComp->GetComponentLocation() + FVector(0.1f),
	                                TargetComp->GetComponentRotation(),Params);
	
	for(const FHitResult& Result : Results)
	{
		if(Result.GetActor() == TargetActor)
		{
			return Result;
		}
	}

	return FHitResult();
}
