// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PlayerController/LakayaAbilityInputSet.h"
#include "LakayaAbility.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class ULakayaInputContext;
class ULakayaAbilityInputSet;
/**
 * @brief Lakaya 게임에서 사용되는 여러가지 기능들을 제공하는 어빌리티 클래스입니다.
 */
UCLASS()
class LAKAYA_API ULakayaAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                          const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	/** 어빌리티 인풋 셋을 통한 입력 바인딩 시에 사용되는 콜백 함수입니다. */
	virtual void AbilityInput(TAbilitySystemInputCallback Function, int32 InputID);

	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                              bool bWasCancelled);

	virtual void NativeCancelAbility(const FGameplayAbilitySpecHandle Handle,
	                                 const FGameplayAbilityActorInfo* ActorInfo,
	                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);

	/** ActorInfo를 통해 향상된 입력 컴포넌트를 찾아 가져옵니다. 반환값은 nullptr일 수 있습니다. */
	static UEnhancedInputComponent* GetEnhancedInputComponent(const FGameplayAbilityActorInfo* ActorInfo);

	/** ActorInfo를 통해 로컬 플레이어를 찾아 가져옵니다. 반환값은 nullptr일 수 있습니다. */
	static ULocalPlayer* GetLocalPlayer(const FGameplayAbilityActorInfo* ActorInfo);

	/** ActorInfo를 통해 향상된 입력 서브시스템을 찾아 가져옵니다. 반환값은 nullptr일 수 있습니다. */
	static UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem(const FGameplayAbilityActorInfo* ActorInfo);

	/** 캐시된 입력 서브시스템을 가져옵니다. 캐시된 입력 서브시스템이 유효하지 않다면, 캐시를 업데이트합니다. 하지만 여전히 nullptr일 수 있습니다. */
	UEnhancedInputLocalPlayerSubsystem* GetCachedInputSubsystem(const FGameplayAbilityActorInfo* ActorInfo);

	/** 로그를 출력합니다. */
	void Log(const FGameplayAbilityActorInfo* ActorInfo, const FString& Message) const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo,
	                           bool bReplicateCancelAbility) override final;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override final;

private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULakayaAbilityInputSet> InputSet;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULakayaInputContext> InputContext;

	UPROPERTY(EditAnywhere)
	uint8 bAddLogOnScreen : 1;

	FLakayaInputHandleContainer InputHandleContainer;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> CachedInputSubsystem;
};
