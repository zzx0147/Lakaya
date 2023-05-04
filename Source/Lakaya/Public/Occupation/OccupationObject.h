#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable/Interactable.h"
#include "OccupationObject.generated.h"

UCLASS()
class LAKAYA_API AOccupationObject : public AInteractable
{
	GENERATED_BODY()

public:
	AOccupationObject();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE APawn* GetInteractingPawn() const { return InteractingPawn; }
	
private:
	virtual void OnInteractionStart(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(const float& Time, APawn* Caller) override;
	virtual void OnCharacterDead(APawn* Caller) override;

private:
	// 플레이어가 움직일 수 있도록 해줍니다.
	void CharacterMovable(APawn* Caller);

	// 플레이어의 움직임을 제한합니다.
	void CharacterImMovable(APawn* Caller);

	// 상호작용을 성공합니다.
	void InteractionSuccess(APawn* Caller);

	// 상호작용이 끝났습니다.
	void OnInteractionFinish(APawn* Caller);
	
private:
	const float MaxInteractionDuration = 3;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCapsuleComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cylinder;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	USphereComponent* TriggerSphere;

#pragma region TODO : ObjectOwner
	// UPROPERTY(ReplicatedUsing = OnRep_BroadCastTeamObject, Transient)
	// EPlayerTeam ObjectOwner;

	// UFUNCTION()
	// void OnRep_BroadCastTeamObject();
	
	// void SetTeamObject(const EPlayerTeam& Team);
#pragma endregion 
private:
	FTimerHandle InteractionTimerHandle;
	FTimerHandle InteractionStateHandle;

private:
	TWeakObjectPtr<class AInteractableCharacter> InteractingCharacter;
	APawn* InteractingPawn;
	float FirstCallerTime = 0;
};