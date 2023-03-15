#pragma once

#include "EngineMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "IndividualEnergy.generated.h"

DECLARE_DELEGATE(FDelegate);

UCLASS()
class LAKAYA_API AIndividualEnergy : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AIndividualEnergy();

protected:
	virtual auto BeginPlay() -> void override;
	virtual void PostInitializeComponents() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

private:
	virtual void InteractionStart(const float& Time, APawn* Caller) override;
	virtual void InteractionStop(const float& Time, APawn* Caller) override;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Box;

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	uint8 StaticEnergyNumber;
	
public:
	UFUNCTION(Category = "Item")
	uint8 GetItemNumber() { return StaticEnergyNumber; }

	UFUNCTION(Category = "Item")
	void GetItem();
};