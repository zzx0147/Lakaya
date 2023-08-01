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
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                          const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	/** 어빌리티 인풋 셋을 통한 입력 바인딩 시에 사용되는 콜백 함수입니다. */
	virtual void AbilityInput(TAbilitySystemInputCallback Function, int32 InputID);

	/** 기존의 EndAbility를 대체하는 이벤트 함수입니다. */
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                              bool bWasCancelled);

	/** 기존의 CancelAbility를 대체하는 이벤트 함수입니다. */
	virtual void NativeCancelAbility(const FGameplayAbilitySpecHandle Handle,
	                                 const FGameplayAbilityActorInfo* ActorInfo,
	                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);

	/** ActorInfo를 통해 향상된 입력 컴포넌트를 찾아 가져옵니다. 반환값은 nullptr일 수 있습니다. */
	static UEnhancedInputComponent* GetEnhancedInputComponent(const FGameplayAbilityActorInfo* ActorInfo);

	/** ActorInfo를 통해 로컬 플레이어를 찾아 가져옵니다. 반환값은 nullptr일 수 있습니다. */
	static ULocalPlayer* GetLocalPlayer(const FGameplayAbilityActorInfo* ActorInfo);

	/** ActorInfo를 통해 향상된 입력 서브시스템을 찾아 가져옵니다. 반환값은 nullptr일 수 있습니다. */
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem(const FGameplayAbilityActorInfo* ActorInfo);

	/** 로그 출력 포맷을 통해 문자열을 생성합니다. */
	FString LogFormat(const FGameplayAbilityActorInfo* ActorInfo, const FString& Message) const;

	/** 로그를 출력합니다. */
	void Log(const FString& Message) const;

	/** 로그포맷을 사용하여 로그를 출력합니다. */
	FORCEINLINE void Log(const FGameplayAbilityActorInfo* ActorInfo, const FString& Message) const
	{
		Log(LogFormat(ActorInfo, Message));
	}

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
	static UEnhancedInputLocalPlayerSubsystem* InternalGetEnhancedInputSubsystem(
		const FGameplayAbilityActorInfo* ActorInfo);

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULakayaInputContext> InputContext;

	UPROPERTY(EditAnywhere)
	uint8 bAddLogOnScreen : 1;

	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> CachedInputSubsystem;
};
