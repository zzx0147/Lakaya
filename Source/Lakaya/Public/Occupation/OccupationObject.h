#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.h"
#include "Interactable/Interactable.h"
#include "OccupationObject.generated.h"

DECLARE_EVENT_OneParam(AOccupationObject, FOccupationStateSignature, EPlayerTeam);

UCLASS()
class LAKAYA_API AOccupationObject : public AInteractable
{
	GENERATED_BODY()

public:
	AOccupationObject();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	virtual void OnInteractionStart(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(const float& Time, APawn* Caller) override;
	virtual void OnCharacterDead(APawn* Caller) override;

public:
	// 상호작용이 끝났습니다.
	void OnInteractionFinish(const APawn* Caller);

	FORCEINLINE APawn* const GetInteractingPawn() const { return InteractingPawn; }
	FORCEINLINE EPlayerTeam const GetObjectTeam() const { return ObjectTeam; }
	
	void SetTeamObject(const EPlayerTeam& Team);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetTeam(EPlayerTeam Team);
	
private:
	UFUNCTION()
	void OnRep_BroadCastTeamObject();

	UPROPERTY(ReplicatedUsing = OnRep_BroadCastTeamObject)
	EPlayerTeam ObjectTeam = EPlayerTeam::None;

	// TWeakObjectPtr<class AInteractableCharacter> InteractingCharacter;
	float FirstCallerTime = 0;
	const float MaxInteractionDuration = 3;

	FOccupationStateSignature FOnOccupationStateSignature;
	
	FTimerHandle InteractionTimerHandle;
	FTimerHandle InteractionStateHandle;
};