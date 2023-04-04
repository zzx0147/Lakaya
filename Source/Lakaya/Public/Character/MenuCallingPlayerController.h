// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "UI/GamePlayCrosshairWidget.h"
#include "UI/GamePlayHealthWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/LoadingWidget.h"
#include "MenuCallingPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AMenuCallingPlayerController : public APlayerController
{
	//TODO: 로드아웃 및 ESC 메뉴를 제외한 다른 기능은 다른 플레이어 컨트롤러에서 하도록 변경해야 합니다.
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
	void CreateGameTimeWidget();
	void CreateScoreBoardWidget();
	void CreateGamePlayCrosshairWidget();
	void CreateGamePlayHealthWidget();
	
private:
	UPROPERTY(EditAnywhere, Category=Input)
	class UInputMappingContext* InterfaceInputContext;

	UPROPERTY(EditAnywhere, Category=Input)
	int8 InterfaceContextPriority;

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* MenuAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* LoadoutAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* ScoreAction;

	FTimerHandle TimerHandle;

public:
	ULoadingWidget* LoadingWidget;
	UGameTimeWidget* GameTimeWidget;
	UGameScoreBoardWidget* GameScoreBoardWidget;
	UGamePlayCrosshairWidget* GamePlayCrosshairWidget;
	UGamePlayHealthWidget* GamePlayHealthWidget;
};