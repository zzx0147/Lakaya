// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "BulletSpreadComponent.generated.h"

DECLARE_EVENT_OneParam(UBulletSpreadComponent, FOnChangeBulletSpreadAmountSignature, const float&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UBulletSpreadComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBulletSpreadComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void AddSpreadAmount(const float& AdditionalAmount);

	virtual float GetTotalSpreadAmount() { return FMath::Max(DefaultSpreadValue+BulletSpreadAmountByMovement + BulletSpreadAmountByFire, 0.0f); }

public:
	FOnChangeBulletSpreadAmountSignature OnChangeBulletSpreadAmountSignature;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float BulletSpreadAmountByMovement;
	float BulletSpreadAmountByFire;

	UPROPERTY(EditDefaultsOnly)
	float DefaultSpreadValue;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxSpreadAmountByFire;
	
	UPROPERTY(EditDefaultsOnly)
	float DecreaseSpreadAmountPerSec;

	UPROPERTY(EditDefaultsOnly)
	float SpreadAmountOnFalling;

	UPROPERTY(EditDefaultsOnly)
	float SpreadAmountOnCrouch;

	UPROPERTY(EditDefaultsOnly)
	float SpreadAmountOnZoom;

	UPROPERTY(EditDefaultsOnly)
	float CharacterVelocityMultiplier;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ZoomTag;
	
	UPROPERTY()
	TWeakObjectPtr<class ALakayaBaseCharacter> LakayaBaseCharacter;
	UPROPERTY()
	TWeakObjectPtr<class UCharacterMovementComponent> CharacterMovementComponent;

	uint8 bIsZoomed : 1;
};
