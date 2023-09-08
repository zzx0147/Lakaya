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
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TWeakObjectPtr<UImage> MiniMap_Img;
};