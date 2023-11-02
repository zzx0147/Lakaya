// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiniMapWidget.h"
#include "Components/RetainerBox.h"
#include "OverlayMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOverlayMinimapWidget : public UMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/**
	 * @brief 미니맵 이미지의 위치와 회전을 업데이트합니다.
	 * @param NewPlayerState 자기 자신의 상태입니다.
	 * @param NewPosition 자기 자신의 위치입니다.
	 */
	virtual void UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const;

	virtual void UpdateEnemyImageRotation(const TWeakObjectPtr<UImage> EnemyImage) const;

	// 점령 구역 이미지의 회전을 업데이트합니다.
	virtual void UpdateAreaImageRotation();

protected:
	// 미니맵 이미지의 부모 패널입니다.
	UPROPERTY()
	TObjectPtr<URetainerBox> RetainerBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AntiAreaImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CenterAreaImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ProAreaImage;
};