#pragma once

#include "EngineMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "IndividualItem.generated.h"

DECLARE_DELEGATE(FDelegate);

UCLASS()
class LAKAYA_API AIndividualItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AIndividualItem();

protected:
	virtual auto BeginPlay() -> void override;
	virtual void PostInitializeComponents() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

private:
	virtual void InteractionStart_Implementation(const float& Time, APawn* Caller);
	virtual void InteractionStop_Implementation(const float& Time, APawn* Caller);
	
public:
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

public:
	int32 GetItemNumber();
	void GetItem();
	
public:
	int32 ItemNumber;

private:
	// UFUNCTION()
	// void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp,
	// 	AActor* OtherActor,
	// 	UPrimitiveComponent* OtherComp,
	// 	int32 OtherBodyIndex,
	// 	bool bFromSweep,
	// 	const FHitResult& SweepResult);
};