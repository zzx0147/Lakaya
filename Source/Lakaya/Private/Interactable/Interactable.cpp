// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Interactable.h"

AInteractable::AInteractable()
{
	Tags.Add("Interactable");

	InteractingPawn = nullptr;
}