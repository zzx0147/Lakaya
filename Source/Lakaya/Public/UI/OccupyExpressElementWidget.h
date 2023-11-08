// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "RadialProgressBar.h"
#include "OccupyExpressElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupyExpressElementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitOccupyExpressElementWidget(const uint8& CaptureId ,const TObjectPtr<UTexture2D> NoneImage) const;
	
	FORCEINLINE TObjectPtr<UImage>& GetInImage() { return InImage; }
	FORCEINLINE TObjectPtr<URadialProgressBar>& GetProgressBar() { return ProgressBar; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetAntiAreaNoneImage() { return AntiAreaNoneImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetCenterAreaNoneImage() { return CenterAreaNoneImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetProAreaNoneImage() { return ProAreaNoneImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetAntiAreaAntiImage() { return AntiAreaAntiImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetAntiAreaProImage() { return AntiAreaProImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetCenterAreaAntiImage() { return CenterAreaAntiImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetCenterAreaProImage() { return CenterAreaProImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetProAreaAntiImage() { return ProAreaAntiImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetProAreaProImage() { return ProAreaProImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetAntiFillImage() { return AntiFillImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetProFillImage() { return ProFillImage; }
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URadialProgressBar> ProgressBar;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProAreaNoneImage;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiFillImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProFillImage;
};