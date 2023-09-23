// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/CanvasPanel.h"
#include "Occupation/Team.h"
#include "UI/MiniMapWidget.h"
#include "Components/Image.h"
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
	UImage* CreatePlayerImage(const ETeam& NewTeam);

private:
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
	void UpdatePlayerPosition();
	
public:
	// 미니맵을 업데이트 여부입니다.
	bool UpdateMinimap;

	TMap<ETeam, TMap<TWeakObjectPtr<ALakayaBasePlayerState>, TWeakObjectPtr<UImage>>> PlayersByMinimap;
	
private:
	TWeakObjectPtr<UCanvasPanel> CanvasPanel;
	
	TWeakObjectPtr<UImage> MinimapImage;

	// TODO : 이미지를 미리 생성해서, 위치를 업데이트 하는 게 아닌
	// 동적으로 플레이어가 입장할 때 본인의 팀 색깔에 맞는 아이콘 이미지를 만들어서,
	// 위치를 업데이트 하는 방식으로 변경해야 합니다.
	TWeakObjectPtr<UImage> PlayerImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* MinimapAntiIcon;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* MinimapProIcon;

	// TODO : 아직 리소스가 나오지 않았습니다.
	UPROPERTY(EditAnywhere)
	UTexture2D* MinimapOwnIcon;

	// TODO : 아직 리소스가 나오지 않았습니다.
	UPROPERTY(EditAnywhere)
	UTexture2D* MinimapDeathIcon;
};