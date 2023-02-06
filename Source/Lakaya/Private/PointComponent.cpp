// Fill out your copyright notice in the Description page of Project Settings.


#include "PointComponent.h"

// Sets default values for this component's properties
UPointComponent::UPointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UPointComponent::BeginPlay()
{
	Super::BeginPlay();

	Point = InitialPoint;
}

void UPointComponent::GainPoint(const uint8& GainingPoint)
{
	Point += GainingPoint;
	OnPointChanged.Broadcast(Point);
}

void UPointComponent::ReturnPoints()
{
	Point = 0;
	OnPointChanged.Broadcast(Point);
}
