#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/CharacterAbility.h"
#include "OverdriveAbility.generated.h"

UCLASS()
class LAKAYA_API UOverdriveAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UOverdriveAbility();

	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	virtual bool ShouldStartRemoteCall() override;
	virtual void StartDelayedAbility() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetEffectMaterial(UMaterialInstanceDynamic* NewEffectMaterial);

private:
	void ApplyBuff();
	void DisableOverdrive();

protected:
	UPROPERTY(EditAnywhere)
	float BaseAbilityDuration;

private:
	TWeakObjectPtr<UMaterialInstanceDynamic> EffectMaterial;
	FTimerHandle OverdriveTimer;
};
