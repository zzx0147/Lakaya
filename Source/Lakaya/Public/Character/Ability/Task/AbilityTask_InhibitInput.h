// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_InhibitInput.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputInhibitedSignature, const int32&, InputID);

/**
 * 
 */
UCLASS()
class LAKAYA_API UAbilityTask_InhibitInput : public UAbilityTask
{
	GENERATED_BODY()

public:
	/** InputID들에 대한 입력을 가로채도록 합니다. 이 어빌리티 태스크가 사라질 때까지 InputID들은 계속 차단됩니다. */
	UFUNCTION(BlueprintCallable,
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_InhibitInput* InhibitInput(UGameplayAbility* OwningAbility, const TSet<int32>& InputIDSet);

	UPROPERTY(BlueprintAssignable)
	FOnInputInhibitedSignature OnInputPressed;

	UPROPERTY(BlueprintAssignable)
	FOnInputInhibitedSignature OnInputReleased;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void OnInputPressInhibition(const int32& InputID);
	void OnInputReleaseInhibition(const int32& InputID);

	TSet<int32> InhibitedInputIDSet;
};
