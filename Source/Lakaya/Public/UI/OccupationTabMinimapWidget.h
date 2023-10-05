// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TabMinimapWidget.h"
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
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) override;
	
private:
	// TODO : 후에 미니맵상에 표시되는 아이콘이 캐릭터와 팀에 따라서 달라집니다.
	
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