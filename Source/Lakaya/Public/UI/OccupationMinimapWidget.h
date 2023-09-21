// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "UI/MiniMapWidget.h"
#include "OccupationMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupationMinimapWidget : public UMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize);

	void UpdatePlayerPosition(const FVector2D& PlayerPosition);

public:
	bool UpdateMinimap;
	
private:
	UImage* MinimapImage;

	UImage* PlayerImage;
};