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
	virtual void InteractionStart(const float& Time, APawn* Caller) override;
	virtual void InteractionStop(const float& Time, APawn* Caller) override;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Box;

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 ItemNumber;
	
public:
	UFUNCTION(Category = "Item")
	int32 GetItemNumber() { return ItemNumber; }

	UFUNCTION(Category = "Item")
	void GetItem();

};