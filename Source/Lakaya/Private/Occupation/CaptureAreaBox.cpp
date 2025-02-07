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

	CaptureAreaWidgetComponent->SetupAttachment(Trigger);
}