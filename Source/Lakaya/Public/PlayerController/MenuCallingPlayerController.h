// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "UI/GamePlayCrosshairWidget.h"
#include "UI/GamePlayHealthWidget.h"
#include "UI/GameResultWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/TeamScoreWidget.h"
#include "Net/UnrealNetwork.h"
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
	template <typename T>
	T* CreateWidgetHelper(const FString& Path)
	{
		UClass* WidgetClass = LoadClass<T>(nullptr, *Path);
		if (WidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is null"), *Path);
			return nullptr;
		}

		T* Widget = CreateWidget<T>(GetWorld(), WidgetClass);
		if (Widget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Widget is null."), *Path);
			return nullptr;
		}
		
		Widget->AddToViewport();
		return Widget;
	}
	
	void CreateLoadingWidget();
	void CreateGameTimeWidget();
	void CreateScoreBoardWidget();
	void CreateGamePlayCrosshairWidget();
	void CreateTeamScoreWidget();
	void CreateGameResultWidget();
	
	void CreateDirectionalDamageIndicator();

	void IndicateStart(FName CauserName, FVector DamageCursorPosition, float time);
	
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
	UTeamScoreWidget* TeamScoreWidget;
	UGameResultWidget* GameResultWidget;
	class UDirectionalDamageIndicator* DirectionalDamageIndicator;
};