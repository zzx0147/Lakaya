// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/LakayaAbilityInputSet.h"

void FLakayaInputHandle::RemoveBinding(UEnhancedInputComponent* InputComponent)
{
	if (!ensure(InputComponent)) return;
	RemoveSingleBinding(Press, InputComponent);
	RemoveSingleBinding(Release, InputComponent);
}

void FLakayaInputHandleContainer::RemoveBindings()
{
	if (InputComponent.IsValid())
	{
		for (auto& Handle : Handles)
		{
			Handle.RemoveBinding(InputComponent.Get());
		}
	}
	Handles.Empty();
	InputComponent.Reset();
}
