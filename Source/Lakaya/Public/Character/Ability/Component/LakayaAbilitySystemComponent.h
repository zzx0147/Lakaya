#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LakayaAbilitySystemComponent.generated.h"

DECLARE_DELEGATE_OneParam(FInputInhibitedDelegate, const int32& /* InhibitedInputID */)

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

	virtual void AbilityLocalInputPressed(int32 InputID) override;
	virtual void AbilityLocalInputReleased(int32 InputID) override;

protected:
	virtual void InitializeComponent() override;
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

private:
	/** 입력을 가로채는 델리게이트 맵입니다. */
	TMap<int32, FInputInhibitedDelegatePair> InputInhibitedDelegates;
};
