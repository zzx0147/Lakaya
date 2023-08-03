// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "CaptureArena.generated.h"

UCLASS()
class LAKAYA_API ACaptureArena : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureArena();

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief 
	 * @param OverlappedComp 겹친 컴포넌트를 나타내는 포인터입니다.
	 * @param OtherActor 겹친 다른 액터를 나타내는 포인터입니다.
	 * @param OtherComp 겹친 다른 컴포넌트를 나타내는 포인터입니다.
	 * @param OtherBodyIndex 겹친 다른 컴포넌트와의 관계를 설명하는 정수 인덱스입니다.
	 * @param bFromSweep 겹침 여부를 나타내는 bool값입니다.
	 * @param SweepResult 겹친 결과를 반환하는 FHitResult 구조체로부터 상수입니다.
	 */
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
		);
	
public:

protected:
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* Trigger;
};