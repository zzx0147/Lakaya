// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TabMinimapWidget.h"
#include "Components/CanvasPanel.h"
#include "Occupation/Team.h"
#include "OccupationTabMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupationTabMinimapWidget : public UTabMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize) override;
	virtual void UpdatePlayerPosition(const ETeam& Team) override;

public:
	/**
	* @brief 미니맵에 모든 플레이어들의 아이콘(이미지)으로 띄우기 위해, 플레이어 인원 수만큼 이미지를 동적 생성합니다.
	* @param NewTeam 팀에 따라, 아이콘의 이미지가 달라지는데, 생성된 팀의 정보입니다.
	* @param bMyPlayer 나 자신인지 아닌지의 여부입니다.
	* @return 동적으로 생성한 이미지를 리턴합니다.
	*/
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) override;
	
private:
	// 위젯의 최상단 CanvasPanel
	UPROPERTY()
	TObjectPtr<UCanvasPanel> ParentPanel;

	// 미니맵상에 Anti팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiIcon;

	// 미니맵상에 Pro팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProIcon;

	UPROPERTY()
	TMap<ETeam, TObjectPtr<UTexture2D>> TeamIcons;
};