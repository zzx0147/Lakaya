// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ImageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UImageTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	virtual void SetText(FText NewText);

private:
	TObjectPtr<class UTextBlock> Text;
	TObjectPtr<class UImage> Image;
};
