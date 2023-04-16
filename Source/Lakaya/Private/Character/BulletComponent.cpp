// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BulletComponent.h"

UBulletComponent::UBulletComponent()
{
	MaxBullets = 30;
}

bool UBulletComponent::CostBullet(const uint16& Value)
{
	if (Bullets < Value) return false;
	Bullets -= Value;
	return true;
}

void UBulletComponent::Reload()
{
	Bullets = MaxBullets;
	OnBulletsChanged.Broadcast(Bullets);
}

void UBulletComponent::OnRep_Bullets()
{
	OnBulletsChanged.Broadcast(Bullets);
}

void UBulletComponent::OnRep_MaxBullets()
{
	OnMaxBulletsChanged.Broadcast(MaxBullets);
}
