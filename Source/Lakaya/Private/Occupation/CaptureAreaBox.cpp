// Fill out your copyright notice in the Description page of Project Settings.


#include "Occupation/CaptureAreaBox.h"

#include "Components/BoxComponent.h"

ACaptureAreaBox::ACaptureAreaBox()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = Trigger;

	CaptureAreaRange = FVector(1000.0f, 1000.0f, 1000.0f);
	
	Trigger->SetBoxExtent(CaptureAreaRange);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACaptureAreaBox::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ACaptureAreaBox::OnOverlapEnd);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(Trigger);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMesh(TEXT("/Game/Dev/DoJun/CaptureArea/CaptureArea_Box"));
	if (BoxMesh.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(BoxMesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BoxMesh Create Failed."));
	}
}
	
void ACaptureAreaBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ACaptureAreaBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}