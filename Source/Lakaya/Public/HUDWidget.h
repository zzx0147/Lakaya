#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

UCLASS()
class LAKAYA_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindMinute(class AIndividualGameMode* GameMinute);
	void BindSecond(class AIndividualGameMode* GameSecond);

protected:
	virtual void NativeConstruct() override;
	void UpdateMinute();
	void UpdateSecond();

private:
	TWeakObjectPtr<class AIndividualGameMode> CurrentMinute;
	TWeakObjectPtr<class AIndividualGameMode> CurrentSecond;

private:
	UPROPERTY()
	class UTextBlock* TextMinute;

	UPROPERTY()
	class UTextBlock* TextSecond;
};