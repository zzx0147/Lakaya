// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BulletComponent.h"

#include "Net/UnrealNetwork.h"

UBulletComponent::UBulletComponent()
{
	Bullets = MaxBullets = 30;
}

void UBulletComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBulletComponent, Bullets);
	DOREPLIFETIME(UBulletComponent, MaxBullets);
}


bool UBulletComponent::CostBullet(const uint16& Value)
{
	if (Bullets < Value) return false;
	Bullets -= Value;
	OnBulletsChanged.Broadcast(Bullets);
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
