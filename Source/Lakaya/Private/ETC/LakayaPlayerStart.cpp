// Fill out your copyright notice in the Description page of Project Settings.


#include "ETC/LakayaPlayerStart.h"
#include "Components/CapsuleComponent.h"

ALakayaPlayerStart::ALakayaPlayerStart(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	const auto Capsule = GetCapsuleComponent();
	Capsule->SetCollisionProfileName(TEXT("PlayerStart"));

}
