#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/CharacterAbility.h"
#include "ClairvoyanceAbility.generated.h"

DECLARE_EVENT_OneParam(UClairvoyanceAbility, FClairvoyanceSignature, bool)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UClairvoyanceAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UClairvoyanceAbility();

	FClairvoyanceSignature OnClairvoyanceChanged;

protected:

	virtual void SetClairvoyanceState(const bool& NewState);

public:
	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	TObjectPtr<class AOutlineManager> GetOutlineManager();
	virtual bool ShouldStartRemoteCall() override;
	virtual void StartDelayedAbility() override;
	virtual void StopDelayedAbility() override;
	virtual void OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime) override;

	TWeakObjectPtr<AOutlineManager> OutlineManager;
	bool bIsClairvoyanceOn;

};
