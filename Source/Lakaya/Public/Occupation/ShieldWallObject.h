#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldWallObject.generated.h"

UCLASS()
class LAKAYA_API AShieldWallObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AShieldWallObject();

protected:
	virtual void BeginPlay() override;
};
