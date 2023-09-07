// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UImage* GetImageElement() const { return MiniMap_Image.Get(); }
	
private:
	TWeakObjectPtr<UImage> MiniMap_Image;
};