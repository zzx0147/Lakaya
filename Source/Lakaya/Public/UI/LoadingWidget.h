#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LoadingWidget.generated.h"

UCLASS()
class LAKAYA_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnChangeJoinedPlayers(uint8 Number, uint8 MaxPlayers);

	UFUNCTION()
	void OnChangeMaxPlayers(uint8 Number, uint8 MaxPlayers);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingWidgetText;
};
