// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/LakayaAbilityInputSet.h"

void ULakayaAbilityInputSet::AddMappingContext(UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	RemoveMappingContext();
	RegisteredInputSubsystem = InputSubsystem;
	if (!ensure(RegisteredInputSubsystem.IsValid())) return;
	InputSubsystem->AddMappingContext(Context, Priority);
}

void ULakayaAbilityInputSet::RemoveMappingContext() const
{
	if (!RegisteredInputSubsystem.IsValid()) return;
	RegisteredInputSubsystem->RemoveMappingContext(Context);
}
