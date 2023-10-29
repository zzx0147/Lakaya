// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OverlayMinimapWidget.h"
#include "IndividualOverlayMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UIndividualOverlayMinimapWidget : public UOverlayMinimapWidget
{
	GENERATED_BODY()

public:
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) override;

	// TODO : 나 자신만의 위치를 업데이트 하도록 수정해야 합니다.
	virtual void UpdatePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FVector2d ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize) override;

	void SetEnemyImage() const;
private:
	virtual void UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const override;
};
