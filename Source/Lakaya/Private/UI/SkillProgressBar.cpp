// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillProgressBar.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "GameFramework/GameStateBase.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

USkillProgressBar::USkillProgressBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Percent = 0.0f;
	EnableTime = -1.0f;
	bUpdateProgressBar = false;
	MaxCoolTime = 3.0f;
	CurrentState = ESkillProgressBarState::None;
	ProgressBarType = ESkillProgressBarType::None;
	bShowProgressOnlyZeroStack = false;
	StartTime = 0.0f;
	UltimateGauge = 0.0f;
	MaxUltimateGauge = 100.0f;
	SkillStack = 0;
	MaxSkillStack = 0;
	SkillKeyImageTextureMap.Emplace(ESkillKey::Q, nullptr);
	SkillKeyImageTextureMap.Emplace(ESkillKey::E, nullptr);
	SkillKeyImageTextureMap.Emplace(ESkillKey::RMB, nullptr);
}

void USkillProgressBar::OnEnableTimeChange(const float& ArgEnableTime)
{
	EnableTime = ArgEnableTime;
	if (EnableTime > GetWorld()->GetGameState()->GetServerWorldTimeSeconds()) bUpdateProgressBar = true;
}

void USkillProgressBar::SetTexture(UTexture2D* NewBackgroundImageTexture, UTexture2D* NewFillImageTexture)
{
	BackgroundImageTexture = NewBackgroundImageTexture;
	FillImageTexture = NewFillImageTexture;
	UpdateWidget();
}

void USkillProgressBar::UpdateWidget()
{
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
			BackgroundImageBrush.SetImageSize(FVector2d(BackgroundImageTexture->GetSizeX(),
			                                            BackgroundImageTexture->GetSizeY()));

			MyStyle.SetBackgroundImage(BackgroundImageBrush);
		}

		SkillProgressBar->SetWidgetStyle(MyStyle);

		SkillProgressBar->SetBarFillType(EProgressBarFillType::BottomToTop);
		SkillProgressBar->SetPercent(Percent);
		// SkillProgressBar->InvalidateLayoutAndVolatility();
		// InvalidateLayoutAndVolatility();
	}
}

void USkillProgressBar::StartCoolTime(const float& ArgStartTime, const float& Duration)
{
	EnableTime = ArgStartTime + Duration;
	StartTime = ArgStartTime;
	MaxCoolTime = Duration;
	if (EnableTime > GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
		CurrentState = ESkillProgressBarState::CoolTime;
}

void USkillProgressBar::StartStackingRegen(const float& ArgStartTime, const float& Duration,
                                           const bool& ArgShowProgressOnlyZeroStack)
{
	bShowProgressOnlyZeroStack = ArgShowProgressOnlyZeroStack;
	EnableTime = ArgStartTime + Duration;
	StartTime = ArgStartTime;
	MaxCoolTime = Duration;
	if (EnableTime > GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
		CurrentState = ESkillProgressBarState::StackingRegen;
}

void USkillProgressBar::OnChangeUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue)
{
	UltimateGauge = NewValue.NewValue;
	SkillProgressBar->SetPercent(UltimateGauge / MaxUltimateGauge);
	
}

void USkillProgressBar::OnChangeMaxUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue)
{
	MaxUltimateGauge = NewValue.NewValue;
	SkillProgressBar->SetPercent(UltimateGauge / MaxUltimateGauge);
}


void USkillProgressBar::OnChangeSkillStackAttribute(const FOnAttributeChangeData& NewValue)
{
	SkillStack = FMath::RoundToInt(NewValue.NewValue);
	UpdateSkillStackImages();
	if (SkillStack >= MaxSkillStack)
	{
		CurrentState = ESkillProgressBarState::None;
	}
}

void USkillProgressBar::OnChangeMaxSkillStackAttribute(const FOnAttributeChangeData& NewValue)
{
	MaxSkillStack = FMath::RoundToInt(NewValue.NewValue);
	CreateSkillStackImages(MaxSkillStack);
	UpdateSkillStackImages();
}

void USkillProgressBar::SetProgressType(const ESkillProgressBarType& NewType)
{
	ProgressBarType = NewType;

	SkillStackPanel->SetVisibility(ProgressBarType == ESkillProgressBarType::StackingRegen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	
	if (ProgressBarType == ESkillProgressBarType::None)
		SetVisibility(ESlateVisibility::Hidden);
	else
		SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USkillProgressBar::SetKey(const ESkillKey& NewKey)
{
	if (SkillKeyImageTextureMap.Contains(NewKey))
		SkillKeyImage->SetBrushFromTexture(SkillKeyImageTextureMap[NewKey]);
}

void USkillProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateWidget();
}

void USkillProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (SkillProgressBar == nullptr) return;

	switch (CurrentState)
	{
	case ESkillProgressBarState::None:
		return;
	case ESkillProgressBarState::CoolTime:
		{
			const float RemainCoolTime = EnableTime - GetWorld()->GetTimeSeconds();
			if (RemainCoolTime < 0)
			{
				SkillProgressBar->SetPercent(1.0f);
				CurrentState = ESkillProgressBarState::None;
				return;
			}
			SkillProgressBar->SetPercent(1.0f - (RemainCoolTime / MaxCoolTime));
			break;
		}
	case ESkillProgressBarState::StackingRegen:
		{
			if (bShowProgressOnlyZeroStack)
			{
				if (SkillStack <= 0)
				{
					const float PassedTime = GetWorld()->GetTimeSeconds() - StartTime;
					const float RemainRegenTime = FMath::Fmod(PassedTime, MaxCoolTime);
					SkillProgressBar->SetPercent(RemainRegenTime / MaxCoolTime);
					// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f"),PassedTime));
					if(RemainRegenTime >= MaxCoolTime - 0.1f)
						SkillStack = 1;
					//UI에서 저장하는 값만 늘어남, 실제로 스킬 스택이 늘어나는 것은 아님, 단순히 클라이언트상에서 쿨타임이 다 돌았는데
					//서버에서 아직 Stack갯수가 늘어난게 리플리케이트되지 않았을 때 프로그래스 바가 다시 0으로 내려가 보이는 것을 막는 장치
				}
				else
				{
					SkillProgressBar->SetPercent(1.0f);
				}
			}
			else
			{
				const float PassedTime = GetWorld()->GetTimeSeconds() - StartTime;
				const float RemainRegenTime = FMath::Fmod(PassedTime, MaxCoolTime);
				SkillProgressBar->SetPercent(RemainRegenTime / MaxCoolTime);
			}
			break;
		}
	case ESkillProgressBarState::Ultimate:
		break;
	}

	// const float RemainCoolTime = EnableTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f"),1.0f - RemainCoolTime / MaxCoolTime));
	// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f : %f"),RemainCoolTime,MaxCoolTime));
}

void USkillProgressBar::CreateSkillStackImages(const uint8& Count)
{
	for (int i = 0; i < SkillStackImages.Num(); ++i)
	{
		SkillStackImages[i]->ConditionalBeginDestroy();
	}
	SkillStackImages.Empty();

	for (int i = 0; i < Count; ++i)
	{
		UImage* NewImage = NewObject<UImage>(this, UImage::StaticClass());
		NewImage->SetBrushFromTexture(SKillStackEmptyTexture);
		NewImage->SetDesiredSizeOverride(FVector2d(SKillStackEmptyTexture->GetSizeX(),
		                                           SKillStackEmptyTexture->GetSizeY()));
		SkillStackPanel->AddChildToHorizontalBox(NewImage);

		UHorizontalBoxSlot* ImageSlot = UWidgetLayoutLibrary::SlotAsHorizontalBoxSlot(NewImage);
		ImageSlot->SetHorizontalAlignment(HAlign_Center);
		FSlateChildSize SlateChildSize;
		SlateChildSize.SizeRule = ESlateSizeRule::Fill;
		ImageSlot->SetSize(SlateChildSize);
		SkillStackImages.Add(NewImage);
	}
}

void USkillProgressBar::UpdateSkillStackImages()
{
	for (int i = 0; i < SkillStackImages.Num(); ++i)
	{
		if (i < SkillStack)
		{
			SkillStackImages[i]->SetBrushFromTexture(SkillStackFillTexture);
		}
		else
		{
			SkillStackImages[i]->SetBrushFromTexture(SKillStackEmptyTexture);
		}
	}
}
