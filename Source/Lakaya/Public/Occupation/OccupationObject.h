// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable/Interactable.h"
#include "OccupationObject.generated.h"

UENUM()
enum class EObjectOwner : uint8
{
	None UMETA(DisplayName = "None"), // 아무도 소유하고 있지 않은 상태
	A UMETA(DisplayName = "A"), // A팀이 소유하고 있는 상태
	B UMETA(DisplayName = "B"), // B팀이 소유하고 있는 상태
};

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
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStart(APawn* Caller) override;
	virtual void OnLocalInteractionStopBegin(APawn* Caller) override;
	virtual void OnServerInteractionStopBegin(const float& Time, APawn* Caller) override;
	virtual void OnInteractionStop(APawn* Caller) override;

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
	UPROPERTY(ReplicatedUsing = OnRep_BroadCastTeamObject)
	EObjectOwner ObjectOwner = EObjectOwner::None;

private:
	UFUNCTION()
	void OnRep_BroadCastTeamObject();

private:
	void SetTeamObject(EObjectOwner Team);
	
private:
	FTimerHandle InteractionTimerHandle;

private:
	APawn* InteractingPawn;
	APawn* Owner;
};