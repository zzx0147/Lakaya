#pragma once
#include "CoreMinimal.h"
#include "PlayerTeam.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable/Interactable.h"
#include "OccupationObject.generated.h"


UCLASS()
class LAKAYA_API AOccupationObject : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AOccupationObject();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void OnInteractionStart(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(const float& Time, APawn* Caller) override;
	virtual void OnCharacterDead(APawn* Caller) override;
	// virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	// virtual void OnInteractionStart(APawn* Caller) override;
	// virtual void OnLocalInteractionStopBegin(APawn* Caller) override;
	// virtual void OnServerInteractionStopBegin(const float& Time, APawn* Caller) override;
	// virtual void OnInteractionStop(APawn* Caller) override;

private:
	void AutomaticInteractionStop();

private:
	const float MaxInteractionDuration = 3;
	float InteractingStartTime;
	float InteractingStopTime;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCapsuleComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Cylinder;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	USphereComponent* TriggerSphere;

private:
	UPROPERTY(ReplicatedUsing = OnRep_BroadCastTeamObject, Transient)
	EPlayerTeam ObjectOwner;

private:
	UFUNCTION()
	void OnRep_BroadCastTeamObject();

private:
	void SetTeamObject(const EPlayerTeam& Team);

private:
	FTimerHandle InteractionTimerHandle;

private:
	TWeakObjectPtr<class AInteractableCharacter> InteractingCharacter;
	APawn* InteractingPawn;

	float RecentInteractionTime = FLT_MAX;
};
