#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "GradeResultWidget.generated.h"

UCLASS()
class LAKAYA_API UGradeResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	TWeakObjectPtr<UImage> AntiTextBoxImage;
	TWeakObjectPtr<UImage> ProTextBoxImage;
	TWeakObjectPtr<UImage> VictoryImage;
	TWeakObjectPtr<UImage> DefeatImage;
};