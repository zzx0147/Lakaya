// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API URadialProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPercent(const float& NewPercent);

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere)
	float Percent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FillImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> EmptyImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> RadialProgressBarImage;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ProgressBarDynamicMaterial;
};
