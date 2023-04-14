#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"
#include "GameTimeWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameTimeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGameTimeWidget(const FObjectInitializer& ObjectInitializer);

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimeWidgetText;

public:
	UFUNCTION()
	void SetGameTimeWidget(EOccupationGameState ChangeGameState);

private:
	UPROPERTY(EditAnywhere)
	FText TimeTextFormat;

	TWeakObjectPtr<AOccupationGameState> OccupationGameState;
};
