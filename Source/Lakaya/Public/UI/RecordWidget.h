// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecordWidget.generated.h"

UCLASS()
class LAKAYA_API URecordWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	URecordWidget(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void NativeConstruct() override;

	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	void RequestShowRecord();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	bool bRequestShowRecord;

	TWeakObjectPtr<class UEOSGameInstance> EOSGameInstance;
};
