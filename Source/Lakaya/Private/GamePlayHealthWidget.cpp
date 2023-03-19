// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlayHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "DamageableCharacter.h"


UGamePlayHealthWidget::UGamePlayHealthWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Health = 100.0f;
	MaximumHealth = 100.0f;
}

void UGamePlayHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName("HPBar_Prb"));
	HealthText = Cast<UTextBlock>(GetWidgetFromName("Health_Text"));
	MaximumHealthText = Cast<UTextBlock>(GetWidgetFromName("MaximumHealth_Text"));

	check(HealthText);
	check(MaximumHealthText);
	check(HealthProgressBar);

	HealthProgressBar->SetPercent(1.0f);

	ADamageableCharacter* DamageableCharacter = Cast<ADamageableCharacter>(GetOwningPlayer()->GetPawn());

	if (DamageableCharacter != nullptr)
	{
		DamageableCharacter->OnHealthReplicated.AddUObject(this, &UGamePlayHealthWidget::OnChangeHealth);
		DamageableCharacter->OnMaximumHealthReplicated.AddUObject(this, &UGamePlayHealthWidget::OnChangeMaximumHealth);
	}
}

void UGamePlayHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
}

void UGamePlayHealthWidget::OnChangeHealth(AActor* Character, const float& NewHealth)
{
	Health = NewHealth;
	HealthText->SetText(FText::AsNumber(floor(Health)));
	UpdateHealthProgressBar();
}

void UGamePlayHealthWidget::OnChangeMaximumHealth(AActor* Character, const float& NewMaximumHealth)
{
	MaximumHealth = NewMaximumHealth;
	MaximumHealthText->SetText(FText::AsNumber(floor(MaximumHealth)));
	UpdateHealthProgressBar();
}

void UGamePlayHealthWidget::UpdateHealthProgressBar()
{
	HealthProgressBar->SetPercent(Health / MaximumHealth);
}

