// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Input/LakayaInputID.h"
#include "LakayaAbilityInputSet.generated.h"

/** 어빌리티 시스템 컴포넌트의 Press, Release 함수의 공통된 시그니쳐입니다. */
typedef TMemFunPtrType<false, UAbilitySystemComponent, void(int32)>::Type TAbilitySystemInputCallback;

/**
 * @brief 입력 바인딩을 위해 필요한 멤버함수의 시그니쳐입니다.
 * @tparam T 멤버함수의 클래스 타입입니다.
 */
template <typename T>
using TInputActionCallback = typename TMemFunPtrType<false, T, void(TAbilitySystemInputCallback, int32)>::Type;

/** InputComponent에 Press, Release가 바인딩되고 생성된 Handle을 담은 구조체입니다. 바인딩을 다시 해제하는 기능을 구현합니다. */
struct FLakayaInputHandle
{
	int32 Press, Release;

	/** 이 핸들에 대응되는 입력 바인딩을 제거합니다. 바인딩 제거가 성공적으로 이뤄진 경우 Handle값이 0이 됩니다. */
	void RemoveBinding(UEnhancedInputComponent* InputComponent);

private:
	FORCEINLINE static void RemoveSingleBinding(int32& Binding, UEnhancedInputComponent* const& InputComp)
	{
		if (InputComp->RemoveBindingByHandle(Binding))
		{
			Binding = 0;
		}
	}
};

/** InputID와 InputAction을 쌍으로 받는 구조체입니다. InputComponent에 입력을 바인딩하는 기능을 제공합니다. */
USTRUCT(BlueprintType)
struct FLakayaAbilityInputInfo : public FLakayaInputID
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Action;

	/**
	 * @brief 입력 바인딩 데이터를 통해 입력 바인딩을 추가합니다.
	 * @param InputComponent 입력 바인딩을 추가할 대상 컴포넌트입니다.
	 * @param Object 입력에 대한 콜백을 받게 될 오브젝트입니다.
	 * @param InputHandler 입력에 대한 콜백함수이며, Object의 멤버함수입니다.
	 */
	template <typename T>
	FLakayaInputHandle BindAction(UEnhancedInputComponent* InputComponent, T* Object,
	                              TInputActionCallback<T> InputHandler) const
	{
		if (!ensure(IsValid(InputComponent) && IsValid(Object)) || !Action) return {};

		auto SingleBinding = [&,InputID = GetInputID()](auto&& Event, auto&& Callback)
		{
			return InputComponent->BindAction(Action, Event, Object, InputHandler, Callback, InputID).GetHandle();
		};

		return {
			SingleBinding(ETriggerEvent::Triggered, &UAbilitySystemComponent::AbilityLocalInputPressed),
			SingleBinding(ETriggerEvent::Completed, &UAbilitySystemComponent::AbilityLocalInputReleased)
		};
	}
};

/** 여러개의 입력 바인딩을 수행하여 반환되는 핸들을 보관하는 컨테이너입니다. 입력 바인딩 해제 기능을 제공합니다. */
struct FLakayaInputHandleContainer
{
	/** 입력 바인딩을 해제합니다. 이 함수가 호출되면 이 컨테이너의 모든 값이 0이 됩니다. */
	void RemoveBindings();

	TWeakObjectPtr<UEnhancedInputComponent> InputComponent;
	TArray<FLakayaInputHandle> Handles;
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
	 * @brief 이 데이터 에셋이 가지고 있는 입력 바인딩 데이터를 통해 입력 바인딩을 추가합니다.
	 * @param InputComponent 입력 바인딩을 추가할 대상 컴포넌트입니다.
	 * @param Object 입력에 대한 콜백을 받게 될 오브젝트입니다.
	 * @param InputHandler 입력에 대한 콜백함수이며, Object의 멤버함수입니다.
	 * @param HandleContainer 입력 바인딩을 추가한 결과를 담을 컨테이너입니다.
	 */
	template <typename T>
	void BindActions(UEnhancedInputComponent* InputComponent, T* Object, TInputActionCallback<T> InputHandler,
	                 FLakayaInputHandleContainer& HandleContainer) const
	{
		if (!ensure(IsValid(InputComponent) && IsValid(Object))) return;

		for (const auto& Binding : AbilityInputBindings)
		{
			HandleContainer.Handles.Emplace(Binding.BindAction(InputComponent, Object, InputHandler));
		}
		HandleContainer.InputComponent = InputComponent;
	}

private:
	/** 추가할 모든 InputID와 입력의 바인딩 정보입니다. */
	UPROPERTY(EditAnywhere)
	TArray<FLakayaAbilityInputInfo> AbilityInputBindings;
};
