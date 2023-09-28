// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilityTask_EnhancedPlayMontageAndWait.generated.h"

/**
 * 기존의 PlayMontageAndWait에 AnimNotify 이벤트를 수신하는 기능을 추가한 어빌리티 태스크입니다.
 */
UCLASS()
class LAKAYA_API UAbilityTask_EnhancedPlayMontageAndWait : public UAbilityTask_PlayMontageAndWait
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPlayMontageAnimNotifyDelegate OnNotifyBegin;

	UPROPERTY(BlueprintAssignable)
	FPlayMontageAnimNotifyDelegate OnNotifyEnd;

	/**
	 * PlayMontageAndWait과 동일한 기능을 수행하며, 추가적으로 AnimNotify 이벤트를 수신할 수 있습니다.
	 * NotifyName, BranchingPointNotifyPayload는 AnimNotify 이벤트를 수신할 때의 인자이므로, 다른 이벤트에서 사용하면 안됩니다.
	 * OnNotifyBegin, OnNotifyEnd 이벤트는 실행되더라도 이 어빌리티 태스크가 종료되거나 하지는 않습니다.
	 * 만약 이들 이벤트에서 어빌리티 태스크를 종료하고 싶다면, 유효한 TaskInstanceName을 지정하고, 해당 이름으로 CancelTaskByInstanceName()을 호출하도록 하세요.
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta = (HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_EnhancedPlayMontageAndWait* EnhancedPlayMontageAndWait(
		UGameplayAbility* InOwningAbility, FName InTaskInstanceName, UAnimMontage* InMontageToPlay, float InRate = 1.f,
		FName InStartSection = NAME_None, bool InbStopWhenAbilityEnds = true,
		float InAnimRootMotionTranslationScale = 1.f, float InStartTimeSeconds = 0.f);

	virtual void Activate() override;

private:
	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	bool ShouldBroadcastAnimNotifyDelegates(const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;

	int32 MontageInstanceID;
};
