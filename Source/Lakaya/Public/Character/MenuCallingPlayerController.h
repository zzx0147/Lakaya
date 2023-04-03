// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "UI/LoadingWidget.h"
#include "MenuCallingPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AMenuCallingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMenuCallingPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void MenuHandler(const FInputActionValue& Value);
	void LoadoutHandler(const FInputActionValue& Value);
	void ScoreHandler(const FInputActionValue& Value);

public:
	void CreateLoadingWidget();
	void CreateScoreBoardWidget();

	// void ReMoveLoadingWidget();
private:
	UPROPERTY(EditAnywhere, Category=Input)
	class UInputMappingContext* InterfaceInputContext;

	UPROPERTY(EditAnywhere, Category=Input)
	int8 InterfaceContextPriority;

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* MenuAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* LoadoutAction;

	UPROPERTY(EditAnywhere)
	UInputAction* ScoreAction;

	FTimerHandle TimerHandle;

public:
	ULoadingWidget* LoadingWidget;
};