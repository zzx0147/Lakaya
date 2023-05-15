// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/BulletFireAbility.h"

// #include "Character/BulletComponent.h"
#include "Character/LakayaBaseCharacter.h"


UBulletFireAbility::UBulletFireAbility()
{
	BulletCost = 1;
}

void UBulletFireAbility::BeginPlay()
{
	Super::BeginPlay();
	// if (const auto Character = GetOwner<ALakayaBaseCharacter>())
	// {
	// 	if (!Character->HasAuthority()) return;
	// 	BulletComponent = Character->GetResourceComponent<UBulletComponent>();
	// 	if (!BulletComponent.IsValid())
	// 		UE_LOG(LogInit, Error, TEXT("Fail to find BulletComponent in UBulletFireAbility!"));
	// }
}

bool UBulletFireAbility::ShouldFire()
{
	// return BulletComponent.IsValid() && BulletComponent->CostBullet(BulletCost);

	return true;
}