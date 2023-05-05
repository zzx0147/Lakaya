// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BulletComponent.h"

#include "Net/UnrealNetwork.h"
#include "UI/GamePlayBulletWidget.h"

UBulletComponent::UBulletComponent()
{
	MaxBullets = 30;
}

void UBulletComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Reload();
	if (const auto Pawn = GetOwner<APawn>())
		if (const auto LocalController = Pawn->GetController<APlayerController>();
			LocalController && LocalController->IsLocalController())
		{
			BulletWidget = CreateWidget<UGamePlayBulletWidget>(LocalController, BulletWidgetClass);
			if (BulletWidget.IsValid())
			{
				BulletWidget->SetMaxBullet(GetMaxBullets());
				BulletWidget->SetRemainBullet(Bullets);

				OnMaxBulletsChanged.AddUObject(BulletWidget.Get(), &UGamePlayBulletWidget::SetMaxBullet);
				OnBulletsChanged.AddUObject(BulletWidget.Get(), &UGamePlayBulletWidget::SetRemainBullet);
			}
		}
}

void UBulletComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UBulletComponent, Bullets, COND_OwnerOnly);
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
