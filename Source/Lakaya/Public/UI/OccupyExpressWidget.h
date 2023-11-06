// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadialProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "OccupyExpressWidget.generated.h"


/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupyExpressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE TObjectPtr<UImage>& GetAntiAreaInImage() { return AntiAreaInImage; }
	FORCEINLINE TObjectPtr<UImage>& GetCenterAreaInImage() { return CenterAreaInImage; }
	FORCEINLINE TObjectPtr<UImage>& GetProAreaInImage() { return ProAreaInImage; }
	
	FORCEINLINE TObjectPtr<URadialProgressBar>& GetAntiAreaBar() { return AntiAreaBar; }
	FORCEINLINE TObjectPtr<URadialProgressBar>& GetCenterAreaBar() { return CenterAreaBar; }
	FORCEINLINE TObjectPtr<URadialProgressBar>& GetProAreaBar() { return ProAreaBar; }

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
	TObjectPtr<UImage> AntiAreaInImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CenterAreaInImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProAreaInImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URadialProgressBar> AntiAreaBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URadialProgressBar> CenterAreaBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URadialProgressBar> ProAreaBar;

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