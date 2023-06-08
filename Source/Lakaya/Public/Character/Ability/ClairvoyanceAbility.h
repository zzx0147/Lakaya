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
	
protected:
	UFUNCTION()
	void OnRep_AbilityStartTime();

public:
	virtual void RemoteAbilityStart(const float& RequestTime) override;
	virtual void OnAliveStateChanged(const bool& AliveState) override;
	virtual void InitializeComponent() override;
	virtual void LocalAbilityStart() override;

	FClairvoyanceSignature OnClairvoyanceChanged;

protected:
	TObjectPtr<class AOutlineManager> GetOutlineManager();
	virtual bool ShouldStartRemoteCall() override;
	virtual void SetClairvoyanceState(const bool& NewState);

private:
	void AbilityStart();
	void AbilityEnd();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_AbilityStartTime, Transient)
	float AbilityStartTime;

	UPROPERTY(EditAnywhere)
	float AbilityDuration;

	UPROPERTY(EditAnywhere)
	float AbilityDelay;
	
	TWeakObjectPtr<AOutlineManager> OutlineManager;

	bool bIsClairvoyanceOn;
private:
	FTimerHandle AbilityStartHandle;
	FTimerHandle AbilityEndHandle;
};
