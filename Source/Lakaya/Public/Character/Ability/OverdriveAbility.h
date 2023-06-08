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

public:
	UFUNCTION(BlueprintCallable)
	void SetEffectMaterial(UMaterialInstanceDynamic* NewEffectMaterial);
	
protected:
	virtual void OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime) override;
	virtual void StartDelayedAbility() override;
	virtual void StopDelayedAbility() override;

public:
	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	virtual bool ShouldStartRemoteCall() override;

private:
	void ApplyBuff();
	void ExpireBuff();
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> EffectMaterial;
};

