// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OverlayMinimapWidget.h"
#include "OccupationOverlayMinimapWidget.generated.h"

// TODO : 팀전에서 공용으로 사용되는 OccupationMinimapWidget을 만들어서 관리해야 합니다.
// TODO : 후에 미니맵상에 표시되는 아이콘이 캐릭터와 팀에 따라서 달라집니다.
/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupationOverlayMinimapWidget : public UOverlayMinimapWidget
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
};