// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillProgressBar.h"

#include "GameFramework/GameStateBase.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "Components/HorizontalBox.h"

USkillProgressBar::USkillProgressBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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
}

void USkillProgressBar::OnEnableTimeChange(const float& ArgEnableTime)
{
	EnableTime = ArgEnableTime;
	if (EnableTime > GetWorld()->GetGameState()->GetServerWorldTimeSeconds()) bUpdateProgressBar = true;
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
	CurrentState = ESkillProgressBarState::StackingRegen;
}

void USkillProgressBar::OnChangeUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue)
{
	UltimateGauge = NewValue.NewValue;
	SetPercent(UltimateGauge / MaxUltimateGauge);
}

void USkillProgressBar::OnChangeMaxUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue)
{
	MaxUltimateGauge = NewValue.NewValue;
	SetPercent(UltimateGauge / MaxUltimateGauge);
}


void USkillProgressBar::OnChangeSkillStackAttribute(const FOnAttributeChangeData& NewValue)
{
	SetSkillStack(NewValue.NewValue);
}

void USkillProgressBar::SetSkillStack_Implementation(const float& NewValue)
{
	SkillStack = FMath::RoundToInt(NewValue);
	if (SkillStack >= MaxSkillStack)
	{
		CurrentState = ESkillProgressBarState::None;
	}
}

void USkillProgressBar::OnChangeMaxSkillStackAttribute(const FOnAttributeChangeData& NewValue)
{
	SetMaxSkillStack(NewValue.NewValue);
}

void USkillProgressBar::SetMaxSkillStack_Implementation(const float& NewValue)
{
	MaxSkillStack = FMath::RoundToInt(NewValue);
}

void USkillProgressBar::SetProgressType(const ESkillProgressBarType& NewType)
{
	ProgressBarType = NewType;
}

void USkillProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	switch (CurrentState)
	{
	case ESkillProgressBarState::None:
		return;
	case ESkillProgressBarState::CoolTime:
		{
			const float RemainCoolTime = EnableTime - GetWorld()->GetTimeSeconds();
			if (RemainCoolTime < 0)
			{
				SetPercent(1.0f);
				CurrentState = ESkillProgressBarState::None;
				return;
			}
			SetPercent(1.0f - (RemainCoolTime / MaxCoolTime));
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
					SetPercent(RemainRegenTime / MaxCoolTime);
					if (RemainRegenTime >= MaxCoolTime - 0.1f)
						SkillStack = 1;
					//UI에서 저장하는 값만 늘어남, 실제로 스킬 스택이 늘어나는 것은 아님, 단순히 클라이언트상에서 쿨타임이 다 돌았는데
					//서버에서 아직 Stack갯수가 늘어난게 리플리케이트되지 않았을 때 프로그래스 바가 다시 0으로 내려가 보이는 것을 막는 장치
				}
				else
				{
					SetPercent(1.0f);
				}
			}
			else
			{
				const float PassedTime = GetWorld()->GetTimeSeconds() - StartTime;
				const float RemainRegenTime = FMath::Fmod(PassedTime, MaxCoolTime);
				// SkillProgressBar->SetPercent(RemainRegenTime / MaxCoolTime);
				SetPercent(RemainRegenTime / MaxCoolTime);
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