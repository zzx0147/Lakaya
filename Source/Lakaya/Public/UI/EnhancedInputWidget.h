// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UEnhancedInputWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void AddInputContext();
	void RemoveInputContext();

protected:
	virtual void SetupInputComponent(class UEnhancedInputComponent* const& InComponent) { return; }
	virtual void InternalAddInputContext(class UEnhancedInputLocalPlayerSubsystem* const& Subsystem) { return; }
	virtual void InternalRemoveInputContext(UEnhancedInputLocalPlayerSubsystem* const& Subsystem) { return; }

	// true이면 AddInputContext가 호출됩니다.
	UPROPERTY(EditAnywhere)
	bool bAutoAddMappingContext;
};
