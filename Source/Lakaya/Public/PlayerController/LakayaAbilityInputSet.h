// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/LakayaAbilitySet.h"
#include "LakayaAbilityInputSet.generated.h"

/** 어빌리티 시스템 컴포넌트의 Press, Release 함수의 공통된 시그니쳐입니다. */
typedef TMemFunPtrType<false, class UAbilitySystemComponent, void(int32)>::Type FAbilitySystemInputCallback;

/**
 * @brief 입력 바인딩을 위해 필요한 멤버함수의 시그니쳐입니다.
 * @tparam T 멤버함수의 클래스 타입입니다.
 */
template <typename T>
using TInputActionCallback = typename TMemFunPtrType<false, T, void(FAbilitySystemInputCallback, int32)>::Type;

USTRUCT(BlueprintType)
struct FLakayaAbilityInputInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Action;

	/** InputAction에 대응되는 입력 키를 지정합니다. */
	UPROPERTY(EditAnywhere)
	ELakayaAbilityInputBind Input;

	/** Input에 InputOffset만큼의 값을 더해 입력이 전달되도록 합니다. */
	UPROPERTY(EditAnywhere)
	int32 InputOffset;

	/**
	 * @brief 입력 바인딩 데이터를 통해 입력 바인딩을 추가합니다.
	 * @param InputComponent 입력 바인딩을 추가할 대상 컴포넌트입니다.
	 * @param Object 입력에 대한 콜백을 받게 될 오브젝트입니다.
	 * @param InputHandler 입력에 대한 콜백함수이며, Object의 멤버함수입니다.
	 */
	template <typename T>
	void BindAction(UEnhancedInputComponent* InputComponent, T* Object, TInputActionCallback<T> InputHandler);

	/**
	 * @brief BindAction을 통해 바인딩 되었던 입력 바인딩을 제거합니다.
	 * @param InputComponent BindAction에서 사용되었던 InputComponent가 아닌 경우 제대로 바인딩이 제거되지 않을 수 있습니다.
	 * InputComponent를 캐싱하지 않는 이유는 메모리 효율성 때문입니다.
	 */
	void RemoveBinding(UEnhancedInputComponent* InputComponent);

private:
	uint32 PressHandle = 0;
	uint32 ReleaseHandle = 0;
};

/**
 * @brief 어빌리티 시스템 컴포넌트의 입력 바인딩을 위한 데이터 에셋입니다.
 */
UCLASS()
class ULakayaAbilityInputSet : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief 이 데이터 에셋의 컨텍스트를 추가합니다.
	 * @param InputSubsystem 컨텍스트를 추가할 대상 서브시스템입니다.
	 */
	void AddMappingContext(UEnhancedInputLocalPlayerSubsystem* InputSubsystem);

	/** 이 데이터 에셋이 추가한 컨텍스트를 제거합니다. */
	void RemoveMappingContext();

	/**
	 * @brief 이 데이터 에셋이 가지고 있는 입력 바인딩 데이터를 통해 입력 바인딩을 추가합니다.
	 * @param InputComponent 입력 바인딩을 추가할 대상 컴포넌트입니다.
	 * @param Object 입력에 대한 콜백을 받게 될 오브젝트입니다.
	 * @param InputHandler 입력에 대한 콜백함수이며, Object의 멤버함수입니다.
	 */
	template <typename T>
	void BindActions(UEnhancedInputComponent* InputComponent, T* Object, TInputActionCallback<T> InputHandler);

	void RemoveActions();

private:
	/** 기본 컨텍스트를 지정합니다. */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> Context;

	/** 컨텍스트의 중요도를 지정합니다. */
	UPROPERTY(EditAnywhere)
	int8 Priority;

	/** 추가할 모든 InputID와 입력의 바인딩 정보입니다. */
	UPROPERTY(EditAnywhere)
	TArray<FLakayaAbilityInputInfo> AbilityInputBindings;

	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> RegisteredInputSubsystem;
	TWeakObjectPtr<UEnhancedInputComponent> RegisteredInputComponent;
};

template <typename T>
void FLakayaAbilityInputInfo::BindAction(UEnhancedInputComponent* InputComponent, T* Object,
                                         TInputActionCallback<T> InputHandler)
{
	if (!ensure(IsValid(InputComponent) && IsValid(Object)) || !Action) return;
	const int32 InputID = static_cast<int32>(Input) + InputOffset;

	PressHandle =
		InputComponent->BindAction(Action, ETriggerEvent::Triggered, Object, InputHandler,
		                           &UAbilitySystemComponent::AbilityLocalInputPressed, InputID).GetHandle();

	ReleaseHandle =
		InputComponent->BindAction(Action, ETriggerEvent::Completed, Object, InputHandler,
		                           &UAbilitySystemComponent::AbilityLocalInputReleased, InputID).GetHandle();
}

template <typename T>
void ULakayaAbilityInputSet::BindActions(UEnhancedInputComponent* InputComponent, T* Object,
                                         TInputActionCallback<T> InputHandler)
{
	if (!ensure(IsValid(InputComponent) && IsValid(Object))) return;
	for (auto& BindInfo : AbilityInputBindings)
	{
		BindInfo.BindAction(InputComponent, Object, InputHandler);
	}
	RegisteredInputComponent = InputComponent;
}
