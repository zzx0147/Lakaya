// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/RenaAnimInstance.h"

#include "Character/Ability/CoolTimedSummonAbility.h"
#include "GameFramework/GameStateBase.h"

URenaAnimInstance::URenaAnimInstance()
{
}

void URenaAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	if(const auto Character = Cast<AArmedCharacter>(TryGetPawnOwner()))
	{
		if (const auto Ability = Character->FindAbility<UCoolTimedSummonAbility>(WeaponAbility))
		{
			Ability->OnPerformTimeNotified.AddUObject(this, &URenaAnimInstance::OnWeaponAbilityPerformTimeNotified);
		}
		if (const auto Ability = Character->FindAbility<UCoolTimedSummonAbility>(Primary))
		{
			Ability->OnPerformTimeNotified.AddUObject(this, &URenaAnimInstance::OnPrimaryAbilityPerformTimeNotified);
		}
		if (const auto Ability = Character->FindAbility<UCoolTimedSummonAbility>(Secondary))
		{
			Ability->OnPerformTimeNotified.AddUObject(this, &URenaAnimInstance::OnSecondaryAbilityPerformTimeNotified);
		}
	}
}

void URenaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void URenaAnimInstance::OnWeaponAbilityPerformTimeNotified(const float& Time)
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
			// 선딜레이 애니메이션이 종료되는 시점부터는 1배속으로 재생합니다.
			WeaponSkillAnimSpeed = 1.f;
			
			// 전체 애니메이션이 종료되는 시간에 bIsWeaponSkill을 false로 바꿔줍니다.
			GetWorld()->GetTimerManager().SetTimer(WeaponAbilityAnimTimer, [this]
			{
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
		GetWorld()->GetTimerManager().SetTimer(WeaponAbilityAnimTimer, [this] { bIsWeaponSkill = false; },
		                                       RemainTime, false);
	}
}

void URenaAnimInstance::OnPrimaryAbilityPerformTimeNotified(const float& Time)
{
	PrimaryAbilityPerformTime = Time;
	auto RemainTime = PrimaryAbilityPerformTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	// 아직 투사체 투척 시간이 도래하지 않은 경우 선딜레이 애니메이션 재생시간동안 배속을 걸어 재생하고, 이후에는 1배속으로 재생합니다.
	if (RemainTime > 0.f)
	{
		WeaponSkillAnimSpeed = PrimaryAbilityPerformDelayAnimDuration / RemainTime;
		bIsPrimarySkill = true;

		GetWorld()->GetTimerManager().SetTimer(PrimaryAbilityAnimTimer, [this]
		{
			// 선딜레이 애니메이션이 종료되는 시점부터는 1배속으로 재생합니다.
			WeaponSkillAnimSpeed = 1.f;
			
			// 전체 애니메이션이 종료되는 시간에 bIsWeaponSkill을 false로 바꿔줍니다.
			GetWorld()->GetTimerManager().SetTimer(PrimaryAbilityAnimTimer, [this]
			{
				bIsPrimarySkill = false;
			}, PrimaryAbilityLateAnimDuration, false);
		}, RemainTime, false);
	}
	// 투사체 투척 시간을 이미 지나버렸지만 아직 전체 애니메이션 시간을 지나지는 않은 경우 애니메이션을 빨리 재생시킵니다.
	else if (-RemainTime < PrimaryAbilityLateAnimDuration)
	{
		// 이제 RemainTime은 전체 애니메이션이 종료되는 시점까지 남은 시간입니다.
		RemainTime += PrimaryAbilityLateAnimDuration;

		// 전체 애니메이션 시간 / 남은시간을 통해 애니메이션 배속을 특정합니다.
		WeaponSkillAnimSpeed = (PrimaryAbilityPerformDelayAnimDuration + PrimaryAbilityLateAnimDuration) / RemainTime;
		bIsPrimarySkill = true;
		GetWorld()->GetTimerManager().SetTimer(PrimaryAbilityAnimTimer, [this] { bIsPrimarySkill = false; },
											   RemainTime, false);
	}
}

void URenaAnimInstance::OnSecondaryAbilityPerformTimeNotified(const float& Time)
{
	SecondaryAbilityPerformTime = Time;
	auto RemainTime = SecondaryAbilityPerformTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	// 아직 투사체 투척 시간이 도래하지 않은 경우 선딜레이 애니메이션 재생시간동안 배속을 걸어 재생하고, 이후에는 1배속으로 재생합니다.
	if (RemainTime > 0.f)
	{
		WeaponSkillAnimSpeed = SecondaryAbilityPerformDelayAnimDuration / RemainTime;
		bIsSecondarySkill = true;

		GetWorld()->GetTimerManager().SetTimer(SecondaryAbilityAnimTimer, [this]
		{
			// 선딜레이 애니메이션이 종료되는 시점부터는 1배속으로 재생합니다.
			WeaponSkillAnimSpeed = 1.f;
			
			// 전체 애니메이션이 종료되는 시간에 bIsWeaponSkill을 false로 바꿔줍니다.
			GetWorld()->GetTimerManager().SetTimer(SecondaryAbilityAnimTimer, [this]
			{
				bIsSecondarySkill = false;
			}, SecondaryAbilityLateAnimDuration, false);
		}, RemainTime, false);
	}
	// 투사체 투척 시간을 이미 지나버렸지만 아직 전체 애니메이션 시간을 지나지는 않은 경우 애니메이션을 빨리 재생시킵니다.
	else if (-RemainTime < SecondaryAbilityLateAnimDuration)
	{
		// 이제 RemainTime은 전체 애니메이션이 종료되는 시점까지 남은 시간입니다.
		RemainTime += SecondaryAbilityLateAnimDuration;

		// 전체 애니메이션 시간 / 남은시간을 통해 애니메이션 배속을 특정합니다.
		WeaponSkillAnimSpeed = (SecondaryAbilityPerformDelayAnimDuration + SecondaryAbilityLateAnimDuration) / RemainTime;
		bIsSecondarySkill = true;
		GetWorld()->GetTimerManager().SetTimer(SecondaryAbilityAnimTimer, [this] { bIsSecondarySkill = false; },
											   RemainTime, false);
	}
}