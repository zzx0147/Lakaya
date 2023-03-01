// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"

#include "Components/SphereComponent.h"

AInteractableActor::AInteractableActor()
{
	InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
	SetRootComponent(InteractionTrigger);
	InteractionTrigger->SetCollisionProfileName(TEXT("Trigger"));
	Tags.Add(TEXT("Interactable"));
}
