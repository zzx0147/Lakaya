// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/CharAnimInstance.h"
#include "WaziAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UWaziAnimInstance : public UCharAnimInstance
{
	GENERATED_BODY()

public:
	UWaziAnimInstance();
	
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bIsWeaponSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bIsPrimarySkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bIsSecondarySkill;

	// 무기 능력 어빌리티의 선딜레이 애니메이션 시간을 지정합니다.
	UPROPERTY(EditAnywhere, Category = "Skill")
	float WeaponAbilityPerformDelayAnimDuration;

	// 선딜레이 이후 무기 능력 어빌리티의 애니메이션 시간을 지정합니다.
	UPROPERTY(EditAnywhere, Category = "Skill")
	float WeaponAbilityLateAnimDuration;

	// Q 어빌리티의 선딜레이 애니메이션 시간을 지정합니다.
	UPROPERTY(EditAnywhere, Category = "Skill")
	float PrimaryAbilityPerformDelayAnimDuration;

	// 선딜레이 이후 Q 어빌리티의 애니메이션 시간을 지정합니다.
	UPROPERTY(EditAnywhere, Category = "Skill")
	float PrimaryAbilityLateAnimDuration;

private:
	void OnWeaponAbilityPerformTimeNotified(const float& Time);
	void OnPrimaryAbilityPerformTimeNotified(const float& Time);

	// 무기 능력으로 소환된 투사체가 실제로 투척이 시작되는 시간을 나타냅니다.
	float WeaponAbilityPerformTime;
	FTimerHandle WeaponAbilityAnimTimer;

	float PrimaryAbilityPerformTime;
	FTimerHandle PrimaryAbilityAnimTimer;
	
};
