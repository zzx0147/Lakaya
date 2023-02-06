// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PointComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointChangedSignature, const uint8&, ChangedPoint);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UPointComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Point)
	uint8 InitialPoint;

public:
	// Sets default values for this component's properties
	UPointComponent();

	void GainPoint(const uint8& GainingPoint);
	void ReturnPoints();

protected:
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnPointChangedSignature OnPointChanged;

	virtual void BeginPlay() override;

private:
	uint8 Point;
};
