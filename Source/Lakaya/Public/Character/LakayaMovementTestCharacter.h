// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/OccupationCharacter.h"
//#include "Movement/LakayaCharacterMovementComponent.h"
#include "LakayaMovementTestCharacter.generated.h"

/**
 *
 */
UCLASS()
class LAKAYA_API ALakayaMovementTestCharacter : public AOccupationCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	class ULakayaCharacterMovementComponent* LakayaCharacterMovementComponent;
private:

public:
	ALakayaMovementTestCharacter(const FObjectInitializer& ObjectInitializer);


	FORCEINLINE ULakayaCharacterMovementComponent* GetLakayaCharacterMovement() const { return LakayaCharacterMovementComponent; }
};
