// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "InGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputMappingContext* InterfaceInputContext;

	UPROPERTY(EditAnywhere, Category=Input)
	int8 InterfaceContextPriority;

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* MenuAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* WeaponLoadoutAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* ArmorLoadoutAction;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	AInGamePlayerController();

private:
	void MenuHandler(const FInputActionValue& Value);
	void WeaponHandler(const FInputActionValue& Value);
	void ArmorHandler(const FInputActionValue& Value);
};
