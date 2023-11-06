// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "OccupyExpressElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupyExpressElementWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> OutImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterAreaNoneImage;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProAreaNoneImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterAreaAntiImage;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProAreaAntiImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiAreaProImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterAreaProImage;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProAreaProImage;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiFillImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProFillImage;
};
