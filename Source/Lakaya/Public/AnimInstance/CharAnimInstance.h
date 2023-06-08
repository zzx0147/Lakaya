// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/ArmedCharacter.h"
#include "Character/InteractableCharacter.h"
#include "Character/Ability/AutoFireAbility.h"
#include "Character/Ability/ResultNotifyFireAbility.h"
#include "Character/Ability/ReloadAbility.h"
#include "CharAnimInstance.generated.h"

UCLASS()
class LAKAYA_API UCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCharAnimInstance();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsAutoFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsReload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interacting")
	bool bIsInteracting;

	// 가장 최근 사격 시간을 나타냅니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecentFireTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireAnimDuration;

	// 무기 능력 어빌리티의 애니메이션 배속을 나타냅니다.
	UPROPERTY(BlueprintReadOnly)
	float WeaponSkillAnimSpeed;

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void OnInteractingActorChanged(AActor* NewInteractingActor);

};
