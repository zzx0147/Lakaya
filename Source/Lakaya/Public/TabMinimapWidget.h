// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Occupation/Team.h"
#include "UI/MiniMapWidget.h"
#include "TabMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UTabMinimapWidget : public UMinimapWidget
{
	GENERATED_BODY()

public:
	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize) override;

	virtual void UpdatePlayerPosition(const ETeam& Team) override;

	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) override;
	
protected:
	// 미니맵상에 자기 자신을 표시하는 아이콘 텍스쳐입니다.
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UTexture2D> OwnIcon;

	// 플레이어가 죽게 되었을 때, 미니맵 상에 표시할 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> DeathIcon;

	// Owner의 소속 팀 입니다.
	// ETeam CurrentTeam;

	// FVector2D MinimapSize;
};