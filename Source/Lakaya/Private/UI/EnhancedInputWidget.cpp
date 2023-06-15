// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EnhancedInputWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void UEnhancedInputWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (const auto Controller = GetOwningPlayer())
	{
		if (const auto CastedComponent = Cast<UEnhancedInputComponent>(Controller->InputComponent))
		{
			SetupInputComponent(CastedComponent);
		}
	}
	if (bAutoAddMappingContext) AddInputContext();
}

void UEnhancedInputWidget::AddInputContext()
{
	if (const auto Subsystem = GetOwningLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		InternalAddInputContext(Subsystem);
}

void UEnhancedInputWidget::RemoveInputContext()
{
	if (const auto Subsystem = GetOwningLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		InternalRemoveInputContext(Subsystem);
}
