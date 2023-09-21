// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	/**
	 * @brief 월드를 미니맵 좌표로 변환합니다.
	 * @param PlayerLocation 현재 본인의 위치입니다.
	 * @param MiniMapSize 미니맵의 사이즈입니다.
	 * @return 
	 */
	FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize);

	/**
	 * @brief 플레이어의 위치를 업데이트 해줍니다.
	 * @param PlayerPosition 미니맵에서 플레이어의 위치를 업데이트 해줍니다.
	 */
	void UpdatePlayerPosition(const FVector2D& PlayerPosition);

public:
	// 미니맵을 업데이트 여부입니다.
	bool UpdateMinimap;
	
private:
	TWeakObjectPtr<class UImage> MinimapImage;

	TWeakObjectPtr<class UImage> PlayerImage;
};