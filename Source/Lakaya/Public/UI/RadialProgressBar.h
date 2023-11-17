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
	UFUNCTION(BlueprintCallable)
	void SetPercent(const float& NewPercent);

	void SetFillImage(const TObjectPtr<UTexture2D> NewImage) const;
	
	FORCEINLINE TObjectPtr<UTexture2D>& GetFillImage() { return FillImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetEmptyImage() { return EmptyImage; }
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
