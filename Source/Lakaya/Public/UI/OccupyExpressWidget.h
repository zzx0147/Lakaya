// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "OccupyExpressWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupyExpressWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FORCEINLINE UProgressBar* GetAntiBar() const { return Anti_Bar.Get(); }
	FORCEINLINE UProgressBar* GetCenterBar() const { return Center_Bar.Get(); }
	FORCEINLINE UProgressBar* GetProBar() const { return Pro_Bar.Get(); }
	FORCEINLINE UTexture* GetOccupyAntiImage() const { return OccupyAntiImage; }
	FORCEINLINE UTexture* GetOccupyProImage() const { return OccupyProImage; }

private:
	UPROPERTY(EditAnywhere, Category = "Image")
	UTexture* OccupyAntiImage;
	
	UPROPERTY(EditAnywhere, Category = "Image")
	UTexture* OccupyProImage;
	
	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> Anti_Bar;
	
	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> Center_Bar;

	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> Pro_Bar;
};