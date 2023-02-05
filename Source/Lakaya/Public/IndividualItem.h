#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "IndividualItem.generated.h"

UCLASS()
class LAKAYA_API AIndividualItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AIndividualItem();

protected:
	virtual auto BeginPlay() -> void override;
	virtual void PostInitializeComponents() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};