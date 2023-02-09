// Fill out your copyright notice in the Description page of Project Settings.


#include "PointCollectingCharacter.h"

#include "PointComponent.h"

APointCollectingCharacter::APointCollectingCharacter()
{
	PointComponent = CreateDefaultSubobject<UPointComponent>(TEXT("Point"));
}
