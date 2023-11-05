#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LakayaAbilitySystemComponent.generated.h"

class ULakayaAbilityCustomImmunity;

DECLARE_DELEGATE_OneParam(FInputInhibitedDelegate, const int32& /* InhibitedInputID */)
DECLARE_MULTICAST_DELEGATE_TwoParams(FImmunedByCustomImmunityDelegate, const FGameplayEffectSpec&,
                                     const ULakayaAbilityCustomImmunity*)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API ULakayaAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	struct FInputInhibitedDelegatePair
	{
		FInputInhibitedDelegate PressedDelegate, ReleasedDelegate;

		FORCEINLINE bool IsBound() const { return PressedDelegate.IsBound() || ReleasedDelegate.IsBound(); }

		FORCEINLINE bool PressedIfBound(const int32& InputID)
		{
			return PressedDelegate.ExecuteIfBound(InputID) || ReleasedDelegate.IsBound();
		}

		FORCEINLINE bool ReleasedIfBound(const int32& InputID)
		{
			return ReleasedDelegate.ExecuteIfBound(InputID) || PressedDelegate.IsBound();
		}
	};

public:
	/** 이 InputID에 대한 입력을 가로채도록 합니다. */
	void AddInputInhibitedDelegate(const int32& InputID, const FInputInhibitedDelegate& PressedDelegate,
	                               const FInputInhibitedDelegate& ReleasedDelegate = {});

	/** 이 InputID가 방해받지 않도록 합니다. */
	void RemoveInputInhibitedDelegate(const int32& InputID);

	/** CustomImmunity에 의해 면역될 때 호출되는 이벤트입니다. */
	FImmunedByCustomImmunityDelegate OnImmunedByCustomImmunity;

	virtual void AbilityLocalInputPressed(int32 InputID) override;
	virtual void AbilityLocalInputReleased(int32 InputID) override;
	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(
		const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey = FPredictionKey()) override;

protected:
	virtual void InitializeComponent() override;
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

private:
	/** 입력을 가로채는 델리게이트 맵입니다. */
	TMap<int32, FInputInhibitedDelegatePair> InputInhibitedDelegates;

	/** 이 ASC에 적용된 면역 조건들입니다. 모두 항상 CDO입니다. */
	TArray<TObjectPtr<ULakayaAbilityCustomImmunity>> CustomImmunities;
};
