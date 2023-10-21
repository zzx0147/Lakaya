// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OverlayMinimapWidget.h"
#include "OverlayOccupationMinimapWidget.generated.h"

// TODO : 팀전에서 공용으로 사용되는 OccupationMinimapWidget을 만들어서 관리해야 합니다.
// TODO : 후에 미니맵상에 표시되는 아이콘이 캐릭터와 팀에 따라서 달라집니다.
/**
 * 
 */
UCLASS()
class LAKAYA_API UOverlayOccupationMinimapWidget : public UOverlayMinimapWidget
{
	GENERATED_BODY()

public:
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) override;
	virtual void UpdatePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) override;
	virtual void HidePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FVector2d ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize) override;
	virtual void UpdatePlayerPosition(const ETeam& Team) override;

	void SetEnemyImage() const;

private:
	virtual void UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const override;

private:
	// 미니맵상에 Anti팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiIcon;

	// 미니맵상에 Pro팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProIcon;

	// 미니맵상에 자기 자신(Anti팀)을 표시하는 아이콘 텍스쳐입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiOwnIcon;

	// 미니맵상에 자기 자신(Pro팀)을 표시하는 아이콘 텍스쳐입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProOwnIcon;

	// AntiIcon, ProIcon을 담는 맵입니다.
	UPROPERTY()
	TMap<ETeam, TObjectPtr<UTexture2D>> TeamIcons;
};