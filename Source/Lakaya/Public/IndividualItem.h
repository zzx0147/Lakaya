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
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};