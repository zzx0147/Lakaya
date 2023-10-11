// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDMinimapWidget.h"
#include "HUDOccupationMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UHUDOccupationMinimapWidget : public UHUDMinimapWidget
{
	GENERATED_BODY()

public:
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FVector2d ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize) override;
	virtual void UpdatePlayerPosition(const ETeam& Team) override;
	virtual void UpdatePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) override;
	
private:
	virtual void UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const override;
};