// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillProgressBar.h"

#include "Components/ProgressBar.h"
#include "GameFramework/GameStateBase.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"

USkillProgressBar::USkillProgressBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Percent = 0.0f;
	EnableTime = -1.0f;
	bUpdateProgressBar = false;
	MaxCoolTime = 3.0f;
}

void USkillProgressBar::OnEnableTimeChange(const float& ArgEnableTime)
{
	EnableTime = ArgEnableTime;
	if(EnableTime > GetWorld()->GetGameState()->GetServerWorldTimeSeconds()) bUpdateProgressBar = true;
}

void USkillProgressBar::SetTexture(UTexture2D* NewBackgroundImageTexture, UTexture2D* NewFillImageTexture)
{
	BackgroundImageTexture = NewBackgroundImageTexture;
	FillImageTexture = NewFillImageTexture;
	UpdateWidget();
}

void USkillProgressBar::UpdateWidget()
{
	SkillProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("SkillProgressBar")));

	if (SkillProgressBar != nullptr)
	{
		FProgressBarStyle MyStyle;
		if (FillImageTexture != nullptr)
		{
			FSlateBrush FillImageBrush;
			
			FillImageBrush.SetResourceObject(FillImageTexture);
			FillImageBrush.SetImageSize(FVector2d(FillImageTexture->GetSizeX(), FillImageTexture->GetSizeY()));
			
			MyStyle.SetFillImage(FillImageBrush);
		}

		if (BackgroundImageTexture != nullptr)
		{
			FSlateBrush BackgroundImageBrush;
			
			BackgroundImageBrush.SetResourceObject(BackgroundImageTexture);
			BackgroundImageBrush.SetImageSize(FVector2d(BackgroundImageTexture->GetSizeX(), BackgroundImageTexture->GetSizeY()));
			
			MyStyle.SetBackgroundImage(BackgroundImageBrush);
		}
		
		SkillProgressBar->SetWidgetStyle(MyStyle);

		SkillProgressBar->SetBarFillType(EProgressBarFillType::BottomToTop);
		SkillProgressBar->SetPercent(Percent);
		// SkillProgressBar->InvalidateLayoutAndVolatility();
		// InvalidateLayoutAndVolatility();
	}
}

void USkillProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	UpdateWidget();
}

void USkillProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(!bUpdateProgressBar) return;

	if(SkillProgressBar == nullptr) return;
	const float RemainCoolTime = EnableTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	if(RemainCoolTime < 0)
	{
		SkillProgressBar->SetPercent(1.0f);
		bUpdateProgressBar = false;
		return;
	}
	
	SkillProgressBar->SetPercent(1.0f - RemainCoolTime / MaxCoolTime);
}
