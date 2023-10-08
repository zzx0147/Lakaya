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
	virtual void UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const override;

private:
	// 미니맵상에 Anti팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiIcon;

	// 미니맵상에 Pro팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProIcon;

	// AntiIcon, ProIcon을 담는 맵입니다.
	UPROPERTY()
	TMap<ETeam, TObjectPtr<UTexture2D>> TeamIcons;
};