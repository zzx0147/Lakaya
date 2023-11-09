// Fill out your copyright notice in the Description page of Project Settings.


#include "Occupation/CaptureAreaCapsule.h"

#include "Components/CapsuleComponent.h"

ACaptureAreaCapsule::ACaptureAreaCapsule()
{
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->InitCapsuleSize(800.0f, 800.0f);
	

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACaptureAreaCapsule::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ACaptureAreaCapsule::OnOverlapEnd);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(Trigger);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleStaticMesh(TEXT("/Game/Dev/DoJun/CaptureArea/CaptureArea_Cylinder"));
	if (CapsuleStaticMesh.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(CapsuleStaticMesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CapsuleStaticMesh Create Failed."));
	}

	CaptureAreaWidgetComponent->SetupAttachment(Trigger);
}

void ACaptureAreaCapsule::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ACaptureAreaCapsule::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}