// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WaziAnimInstance.h"

#include "Character/Ability/ClairvoyanceAbility.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Character/Ability/OverdriveAbility.h"
#include "GameFramework/GameStateBase.h"

UWaziAnimInstance::UWaziAnimInstance()
{
}

void UWaziAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (const auto Character = Cast<AArmedCharacter>(TryGetPawnOwner()))
	{
		if (const auto Ability = Character->FindAbility<UCoolTimedSummonAbility>(WeaponAbility))
		{
			Ability->OnAbilityStartTimeNotified.
			         AddUObject(this, &UWaziAnimInstance::OnWeaponAbilityPerformTimeNotified);
		}
		if (const auto PrimaryAbility = Character->FindAbility<UOverdriveAbility>(Primary))
		{
			PrimaryAbility->OnAbilityStartTimeNotified.AddUObject(
				this, &UWaziAnimInstance::OnOverdriveStartTimeNotified);
		}
		if (const auto SecondaryAbility = Character->FindAbility<UClairvoyanceAbility>(Secondary))
		{
			SecondaryAbility->OnAbilityStartTimeNotified.AddUObject(
				this, &UWaziAnimInstance::OnClairvoyanceStartTimeNotified);
		}
	}
}

void UWaziAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}


void UWaziAnimInstance::OnWeaponAbilityPerformTimeNotified(const float& Time)
{
	WeaponAbilityPerformTime = Time;
	auto RemainTime = WeaponAbilityPerformTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	// 아직 투사체 투척 시간이 도래하지 않은 경우 선딜레이 애니메이션 재생시간동안 배속을 걸어 재생하고, 이후에는 1배속으로 재생합니다.
	if (RemainTime > 0.f)
	{
		WeaponSkillAnimSpeed = WeaponAbilityPerformDelayAnimDuration / RemainTime;
		bIsWeaponSkill = true;

		GetWorld()->GetTimerManager().SetTimer(WeaponAbilityAnimTimer, [this]
		{
			if (this == nullptr) return;

			// 선딜레이 애니메이션이 종료되는 시점부터는 1배속으로 재생합니다.
			WeaponSkillAnimSpeed = 1.f;

			// 전체 애니메이션이 종료되는 시간에 bIsWeaponSkill을 false로 바꿔줍니다.
			GetWorld()->GetTimerManager().SetTimer(WeaponAbilityAnimTimer, [this]
			{
				if (this == nullptr) return;

				bIsWeaponSkill = false;
			}, WeaponAbilityLateAnimDuration, false);
		}, RemainTime, false);
	}
	// 투사체 투척 시간을 이미 지나버렸지만 아직 전체 애니메이션 시간을 지나지는 않은 경우 애니메이션을 빨리 재생시킵니다.
	else if (-RemainTime < WeaponAbilityLateAnimDuration)
	{
		// 이제 RemainTime은 전체 애니메이션이 종료되는 시점까지 남은 시간입니다.
		RemainTime += WeaponAbilityLateAnimDuration;

		// 전체 애니메이션 시간 / 남은시간을 통해 애니메이션 배속을 특정합니다.
		WeaponSkillAnimSpeed = (WeaponAbilityPerformDelayAnimDuration + WeaponAbilityLateAnimDuration) / RemainTime;
		bIsWeaponSkill = true;
		GetWorld()->GetTimerManager().SetTimer(WeaponAbilityAnimTimer, [this]
		{
			if (this == nullptr) return;
			bIsWeaponSkill = false;
		}, RemainTime, false);
	}
}

void UWaziAnimInstance::OnOverdriveStartTimeNotified(const float& Time)
{
	const auto RemainTime = Time - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	bIsPrimarySkill = RemainTime > 0.f;
	GetWorld()->GetTimerManager().SetTimer(PrimaryAbilityTimer, [this]
	{
		if (this == nullptr) return;
		bIsPrimarySkill = false;
	}, RemainTime, false);
}

void UWaziAnimInstance::OnClairvoyanceStartTimeNotified(const float& Time)
{
	const auto RemainTime = Time - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	bIsSecondarySkill = RemainTime > 0.f;
	GetWorld()->GetTimerManager().SetTimer(SecondaryAbilityTimer, [this]
	{
		if (this == nullptr) return;
		bIsSecondarySkill = false;
	}, RemainTime, false);
}
