// Fill out your copyright notice in the Description page of Project Settings.

#include "Occupation/CaptureArena.h"

#include "Character/ArmedCharacter.h"
#include "Components/BoxComponent.h"

ACaptureArena::ACaptureArena()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));

	SetRootComponent(Trigger);

	Trigger->InitBoxExtent(FVector(200.f, 200.f, 200.f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArena::OnOverlapBegin);
	
	bReplicates = true;
}

void ACaptureArena::BeginPlay()
{
	Super::BeginPlay();
}

void ACaptureArena::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌한 액터가 캐릭터인지 확인합니다.
		const AArmedCharacter* OverlappedArmedCharacter = Cast<AArmedCharacter>(OtherActor);

		if (OverlappedArmedCharacter)
		{
			// 겹친 액터가 캐릭터입니다.
			// UE_LOG(LogTemp, Log, TEXT("Trigger Overlaped"));
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Trigger Overlaped"));
		}
		else
		{
			// 겹친 액터는 캐릭터가 아닙니다.
			// UE_LOG(LogTemp, Log, TEXT("Trigger Overlapped by non-ArmedCharacter"));
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Trigger Overlapped by non-ArmedCharacter"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Same Trigger"));
	}
}