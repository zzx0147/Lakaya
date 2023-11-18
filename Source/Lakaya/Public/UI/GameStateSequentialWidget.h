// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStateSequentialWidget.generated.h"

class UCommonActivatableWidget;
class UIntroWidget;
class UGameLobbyCharacterSelectWidget;
class ULoadingWidget;
class UCommonActivatableWidgetSwitcher;

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameStateSequentialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintGetter)
	UCommonActivatableWidgetSwitcher* GetWidgetSwitcher() const { return WidgetSwitcher; }

	UFUNCTION(BlueprintGetter)
	ULoadingWidget* GetLoadingWidget() const { return LoadingWidget; }

	UFUNCTION(BlueprintGetter)
	UGameLobbyCharacterSelectWidget* GetCharacterSelectWidget() const { return CharacterSelectWidget; }

	UFUNCTION(BlueprintGetter)
	UIntroWidget* GetGameIntroWidget() const { return GameIntroWidget; }

	UFUNCTION(BlueprintGetter)
	UCommonActivatableWidget* GetInGameOverlayWidget() const { return InGameOverlayWidget; }

	/**
	 * 해당 위젯에 스위처에 보여지도록 합니다.
	 * @param bForce true이면 스위처의 자식이 아니던 위젯은 자식으로 추가해서 스위칭합니다, 하지만 비활성화될 때 다시 제거되지는 않습니다.
	 */
	UFUNCTION(BlueprintCallable)
	void SwitchTo(UCommonActivatableWidget* Widget, bool bForce = false);

	UFUNCTION(BlueprintCallable)
	void SwitchToLoading();

	UFUNCTION(BlueprintCallable)
	void SwitchToCharacterSelect();

	UFUNCTION(BlueprintCallable)
	void SwitchToGameIntro();

	UFUNCTION(BlueprintCallable)
	void SwitchToInGameOverlay();

private:
	UPROPERTY(BlueprintGetter="GetWidgetSwitcher", meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(BlueprintGetter="GetLoadingWidget", meta=(BindWidgetOptional))
	TObjectPtr<ULoadingWidget> LoadingWidget;

	UPROPERTY(BlueprintGetter="GetCharacterSelectWidget", meta=(BindWidgetOptional))
	TObjectPtr<UGameLobbyCharacterSelectWidget> CharacterSelectWidget;

	UPROPERTY(BlueprintGetter="GetGameIntroWidget", meta=(BindWidgetOptional))
	TObjectPtr<UIntroWidget> GameIntroWidget;

	UPROPERTY(BlueprintGetter="GetInGameOverlayWidget", meta=(BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidget> InGameOverlayWidget;
};
