#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

UCLASS()
class LAKAYA_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void UpdateTimerText();

	FTimerHandle TimerHandle;
	class UTextBlock* MinuteText;
	class UTextBlock* SecondText;
};