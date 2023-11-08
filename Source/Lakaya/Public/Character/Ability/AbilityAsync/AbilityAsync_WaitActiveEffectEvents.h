// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync.h"
#include "AbilityAsync_WaitActiveEffectEvents.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API UAbilityAsync_WaitActiveEffectEvents : public UAbilityAsync
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActiveEffectCanceled);

	UPROPERTY(BlueprintAssignable)
	FActiveEffectCanceled OnCanceled;

protected:
	template <typename T>
	static T* NewActiveEffectAsync(FActiveGameplayEffectHandle& InActiveHandle)
	{
		const auto MyObj = NewObject<T>();
		MyObj->SetAbilitySystemComponent(InActiveHandle.GetOwningAbilitySystemComponent());
		MyObj->ActiveHandle = InActiveHandle;
		return MyObj;
	}

	FORCEINLINE const FActiveGameplayEffectHandle& GetActiveHandle() const { return ActiveHandle; }

private:
	FActiveGameplayEffectHandle ActiveHandle;
};

UCLASS()
class UAbilityAsync_WaitActiveEffectRemoved : public UAbilityAsync_WaitActiveEffectEvents
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActiveEffectRemoved, const FGameplayEffectRemovalInfo&, RemovalInfo);

	UPROPERTY(BlueprintAssignable)
	FActiveEffectRemoved OnRemoved;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="TRUE"))
	static UAbilityAsync_WaitActiveEffectRemoved* WaitActiveEffectRemoved(FActiveGameplayEffectHandle InActiveHandle);

	virtual void Activate() override;
	virtual void EndAction() override;

private:
	void OnActiveEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo);

	FDelegateHandle OnActiveEffectRemovedDelegateHandle;
};

UCLASS()
class UAbilityAsync_WaitActiveEffectStackCountChanged : public UAbilityAsync_WaitActiveEffectEvents
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActiveEffectStackChanged, int32, NewStackCount, int32, OldStackCount);

	UPROPERTY(BlueprintAssignable)
	FActiveEffectStackChanged OnStackChanged;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="TRUE"))
	static UAbilityAsync_WaitActiveEffectStackCountChanged* WaitActiveEffectStackCountChanged(
		FActiveGameplayEffectHandle InActiveHandle, bool bTriggerOnce = false);

	virtual void Activate() override;
	virtual void EndAction() override;

private:
	void OnActiveEffectStackChanged(FActiveGameplayEffectHandle Handle, int32 NewStackCount, int32 OldStackCount);

	FDelegateHandle OnActiveEffectStackChangedDelegateHandle;
	bool bIsTriggerOnce;
};

UCLASS()
class UAbilityAsync_WaitActiveEffectTimeChanged : public UAbilityAsync_WaitActiveEffectEvents
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActiveEffectTimeChaged, float, NewStartTime, float, NewDuration);

	UPROPERTY(BlueprintAssignable)
	FActiveEffectTimeChaged OnTimeChanged;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="TRUE"))
	static UAbilityAsync_WaitActiveEffectTimeChanged* WaitActiveEffectTimeChanged(
		FActiveGameplayEffectHandle InActiveHandle, bool bTriggerOnce = false);

	virtual void Activate() override;
	virtual void EndAction() override;

private:
	void OnActiveEffectTimeChanged(FActiveGameplayEffectHandle Handle, float NewStartTime, float NewDuration);

	FDelegateHandle OnActiveEffectTimeChangedDelegateHandle;
	bool bIsTriggerOnce;
};

UCLASS()
class UAbilityAsync_WaitActiveEffectInhibitionChanged : public UAbilityAsync_WaitActiveEffectEvents
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActiveEffectInhibitionChanged, bool, bIsInhibited);

	UPROPERTY(BlueprintAssignable)
	FActiveEffectInhibitionChanged OnInhibitionChanged;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="TRUE"))
	static UAbilityAsync_WaitActiveEffectInhibitionChanged* WaitActiveEffectInhibitionChanged(
		FActiveGameplayEffectHandle InActiveHandle, bool bTriggerOnce = false);

	virtual void Activate() override;
	virtual void EndAction() override;

private:
	void OnActiveEffectInhibitionChanged(FActiveGameplayEffectHandle Handle, bool bIsInhibited);

	FDelegateHandle OnActiveEffectInhibitionChangedDelegateHandle;
	bool bIsTriggerOnce;
};
