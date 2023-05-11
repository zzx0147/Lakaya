#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ImageTextWidget.generated.h"

UCLASS()
class LAKAYA_API UImageTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UImageTextWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	virtual void SetText(FText NewText);

private:

	TObjectPtr<class UTextBlock> TextElement;
	TObjectPtr<class UImage> ImageElement;
	FText TextData;
};
