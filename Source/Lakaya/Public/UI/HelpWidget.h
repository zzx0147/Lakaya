#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelpWidget.generated.h"

UCLASS()
class LAKAYA_API UHelpWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};