// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/LakayaAbilityInputSet.h"

void FLakayaAbilityInputInfo::RemoveBinding(UEnhancedInputComponent* InputComponent)
{
	if (!IsValid(InputComponent)) return;

	static auto RemoveBind = [](uint32& Binding, UEnhancedInputComponent* const& InputComp)
	{
		if (Binding == 0) return;
		InputComp->RemoveBindingByHandle(Binding);
		Binding = 0;
	};

	RemoveBind(PressHandle, InputComponent);
	RemoveBind(ReleaseHandle, InputComponent);
}

void ULakayaAbilityInputSet::AddMappingContext(UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	if (!ensure(IsValid(InputSubsystem))) return;
	RemoveMappingContext();
	InputSubsystem->AddMappingContext(Context, Priority);
	RegisteredInputSubsystem = InputSubsystem;
}

void ULakayaAbilityInputSet::RemoveMappingContext()
{
	if (!RegisteredInputSubsystem.IsValid()) return;
	RegisteredInputSubsystem->RemoveMappingContext(Context);
	RegisteredInputSubsystem.Reset();
}

void ULakayaAbilityInputSet::RemoveActions()
{
	if (!RegisteredInputComponent.IsValid()) return;
	for (auto& BindInfo : AbilityInputBindings)
	{
		BindInfo.RemoveBinding(RegisteredInputComponent.Get());
	}
}
