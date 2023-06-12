#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.h"
#include "Interactable/Interactable.h"
#include "OccupationObject.generated.h"

DECLARE_EVENT_OneParam(AOccupationObject, FOccupationStateSignature, EPlayerTeam);
DECLARE_EVENT_OneParam(AOccupationObject, FOccupationInteractingActorSignature, APawn*)

UCLASS()
class LAKAYA_API AOccupationObject : public AInteractable
{
	GENERATED_BODY()

public:
	explicit AOccupationObject(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * @brief 오브젝트가 캐릭터 요청에 의해 상호작용을 시작합니다.
	 * @param Time 캐릭터가 상호작용을 요청한 시간입니다.
	 * @param Caller 상호작용을 요청한 캐릭터입니다.
	 */
	virtual void OnInteractionStart(const float& Time, APawn* Caller) override;

	/**
	 * @brief 오브젝트가 캐릭터 요청에 의해 상호작용을 중단합니다.
	 * @param Time 캐릭터가 상호작용을 요청한 시간입니다.
	 * @param Caller 상호작용을 요청한 캐릭터입니다.
	 */
	virtual void OnInteractionStop(const float& Time, APawn* Caller, EInteractionState NewState) override;

	/**
	 * @brief 상호작용을 성공했을 때 실행됩니다.
	 * @param Caller 상호작용을 요청한 캐릭터입니다.
	 */
	virtual void OnInteractionFinish(APawn* Caller) override;

	virtual void OnCharacterDead(APawn* Caller) override;

public:
	// 상호작용에 성공했을 때, 실행되는 함수입니다.

	FORCEINLINE APawn* GetInteractingPawn() const { return InteractingPawn; }
	FORCEINLINE EPlayerTeam GetObjectTeam() const { return ObjectTeam; }
	FORCEINLINE APawn* GetOwnerPlayer() const { return OwnerPlayer.Get(); }
	
	void SetTeamObject(const EPlayerTeam& Team);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetTeam(EPlayerTeam Team);

	FOccupationInteractingActorSignature OnInteractingActorSignature;
	
private:
	UFUNCTION()
	void OnRep_BroadCastTeamObject();

	UPROPERTY(ReplicatedUsing = OnRep_BroadCastTeamObject)
	EPlayerTeam ObjectTeam = EPlayerTeam::None;

	float FirstCallerTime = 0;
	const float MaxInteractionDuration = 3;

	FOccupationStateSignature OnOccupationStateSignature;
	
	FTimerHandle InteractionTimerHandle;
	FTimerHandle InteractionStateHandle;
};