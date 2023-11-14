#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LoadingWidget.generated.h"

UCLASS()
class LAKAYA_API ULoadingWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerFull();
	
public:
	virtual void SetPlayerNumber(const uint8& PlayerCount);
	virtual void SetMaximumPlayerNumber(const uint8& PlayerCount);

private:

	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> LoadingWidgetText;
	
	
	uint8 MaxPlayerCount;
};
