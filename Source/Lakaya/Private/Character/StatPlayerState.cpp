// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StatPlayerState.h"

#include "Engine/DamageEvents.h"


float AStatPlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                   AActor* DamageCauser)
{
	float ReducedDamage = DamageAmount;
	if (DamageEvent.IsOfType(FDamageEvent::ClassID)) ReducedDamage = ApplyReduceStatToDamage(DamageAmount);
	//TODO: 다른 데미지 이벤트가 추가되는 경우, 여기에서도 늘려줘야 데미지 경감 로직이 적용됩니다.
	return Super::TakeDamage(ReducedDamage, DamageEvent, EventInstigator, DamageCauser);
}

float AStatPlayerState::GetMaxHealth() const
{
	//TODO: 체력 관련 버프를 적용하여 반환하도록 합니다.
	return Super::GetMaxHealth();
}

float AStatPlayerState::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent,
                                                AController* EventInstigator, AActor* DamageCauser)
{
	const float PointDamage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);
	return ApplyReduceStatToDamage(PointDamage);
}

float AStatPlayerState::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
                                                 AController* EventInstigator, AActor* DamageCauser)
{
	const float RadialDamage =
		Super::InternalTakeRadialDamage(Damage, RadialDamageEvent, EventInstigator, DamageCauser);
	return ApplyReduceStatToDamage(RadialDamage);
}

float AStatPlayerState::ApplyReduceStatToDamage(const float& Damage) const
{
	//TODO: 현재 스탯에 따라 데미지를 경감하는 로직을 작성합니다.
	return Damage;
}
