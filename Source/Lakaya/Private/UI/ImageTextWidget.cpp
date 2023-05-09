// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ImageTextWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UImageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
	Image = Cast<UImage>(GetWidgetFromName(TEXT("Image")));
}

void UImageTextWidget::SetText(FText NewText)
{
	if (Text != nullptr)
	{
		Text->SetText(NewText);
	}
}
