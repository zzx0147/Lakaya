#define DO_CHECK 1

#include "UI/ImageTextWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


UImageTextWidget::UImageTextWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UImageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TextElement = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextElement")));
	ImageElement = Cast<UImage>(GetWidgetFromName(TEXT("ImageElement")));

	check(TextElement != nullptr);
	check(ImageElement != nullptr);

	TextElement->SetText(TextData);
}

void UImageTextWidget::SetText(FText NewText)
{
	//TextElement = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextElement")));
	TextData = NewText;
	if (TextElement != nullptr)
	{
		TextElement->SetText(TextData);
	}
}
