#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/CharacterAbility.h"
#include "OverdriveAbility.generated.h"

DECLARE_EVENT_OneParam(UOverdriveAbility, FOverdriveSignature, bool)

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
	virtual void SetOverdriveState(const bool& NewState);

public:
	virtual void OnAliveStateChanged(const bool& AliveState) override;

	FOverdriveSignature OnOverdriveChanged;

protected:
	virtual bool ShouldStartRemoteCall() override;
	
	bool bIsOverdriveOn;

private:
	void ApplyBuff();
	void ExpireBuff();
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> EffectMaterial;
};

