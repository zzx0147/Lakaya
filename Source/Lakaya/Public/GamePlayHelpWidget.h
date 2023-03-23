// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHelpWidget.generated.h"

/**
 *
 */

class UCanvasPanel;

UCLASS()
class LAKAYA_API UGamePlayHelpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayHelpWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UCanvasPanel* HelpBodyPanel;
};
