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

protected:
	virtual void NativeConstruct() override;
	
public:
	void InitOccupyExpressElementWidget(const uint8& CaptureId) const;
	
	FORCEINLINE TObjectPtr<UImage>& GetInImage() { return InImage; }
	FORCEINLINE TObjectPtr<UImage>& GetOutImage() { return OutImage;}
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

	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingOutImage() { return FloatingOutImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingAntiAreaNoneImage() { return FloatingAntiAreaNoneImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingAntiAreaAntiImage() { return FloatingAntiAreaAntiImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingAntiAreaProImage() { return FloatingAntiAreaProImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingCenterAreaNoneImage() { return FloatingCenterAreaNoneImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingCenterAreaAntiImage() { return FloatingCenterAreaAntiImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingCenterAreaProImage() { return FloatingCenterAreaProImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingProAreaNoneImage() { return FloatingProAreaNoneImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingProAreaAntiImage() { return FloatingProAreaAntiImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingProAreaProImage() { return FloatingProAreaProImage; }

	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingAntiFillImage() { return FloatingAntiFillImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingProFillImage() { return FloatingProFillImage; }
	FORCEINLINE TObjectPtr<UTexture2D>& GetFloatingEmptyImage() { return FloatingEmptyImage; }
private:
#pragma region OccupyExpressElementWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> OutImage;
	
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
#pragma endregion

#pragma region Floating 관련 OccupyExpressElementWidget
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingOutImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingAntiAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingAntiAreaAntiImage;

	UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> FloatingAntiAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingCenterAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingCenterAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingCenterAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingProAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingProAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingProAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingAntiFillImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingProFillImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> FloatingEmptyImage;
#pragma endregion
};