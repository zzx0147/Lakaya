// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Occupation/CaptureArea.h"
#include "CaptureAreaBox.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ACaptureAreaBox : public ACaptureArea
{
	GENERATED_BODY()

public:
	ACaptureAreaBox();

private:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;	

public:
	
private:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;
	
	UPROPERTY(EditAnywhere)
	FVector CaptureAreaRange;
};
