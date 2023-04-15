// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "Components/TextBlock.h"
#include "GameTimeWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameTimeWidget : public UMatchStateWidget
{
	GENERATED_BODY()

public:
	explicit UGameTimeWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool OnMatchStart() override;

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimeWidgetText;

	UPROPERTY(EditAnywhere)
	FText TimeTextFormat;

	TWeakObjectPtr<class AOccupationGameState> OccupationGameState;
};
