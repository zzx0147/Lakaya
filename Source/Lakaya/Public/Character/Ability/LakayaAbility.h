// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LakayaAbility.generated.h"

class UEnhancedInputComponent;
class UEnhancedInputLocalPlayerSubsystem;
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

	/** 서버에게 타겟 데이터를 전달합니다. */
	void ServerSetReplicatedTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                   const FGameplayTag& GameplayTag = FGameplayTag()) const;

	/** 현재 어빌리티 핸들과 ActivationInfo를 통해 타겟 데이터 이벤트를 가져옵니다. */
	FAbilityTargetDataSetDelegate& GetTargetDataDelegate() const;

	const FGameplayAbilityActivationInfo& GetCurrentActivationInfoRef() const;

	void ConsumeTargetData() const;

	UFUNCTION(BlueprintCallable)
	void TargetDataScope();

	/** 클라이언트로 타겟 데이터가 전달되면 호출되는 이벤트 함수입니다. */
	UFUNCTION(BlueprintNativeEvent)
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag);

	UFUNCTION(BlueprintNativeEvent)
	FGameplayAbilityTargetDataHandle MakeTargetData();

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

	/** 로그가 PIE에서도 표시되도록 하는 기능입니다. */
	UPROPERTY(EditAnywhere)
	uint8 bAddLogOnScreen : 1;

	/** InputReleased에서 어빌리티를 종료시키도록 하는 기능입니다. */
	UPROPERTY(EditAnywhere)
	uint8 bEndOnInputRelease : 1;

	UPROPERTY(EditAnywhere)
	float ServerTargetDataTimeOut;

	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> CachedInputSubsystem;
	FDelegateHandle TargetDataDelegateHandle;
	FTimerHandle TargetDataTimerHandle;
};
