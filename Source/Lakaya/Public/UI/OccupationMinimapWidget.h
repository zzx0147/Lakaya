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
	/**
	 * @brief 미니맵에 모든 플레이어들의 아이콘(이미지)으로 띄우기 위해, 플레이어 인원 수만큼 이미지를 동적 생성합니다.
	 * @param NewTeam 팀에 따라, 아이콘의 이미지가 달라지는데, 생성된 팀의 정보입니다.
	 * @param bMyPlayer 나 자신인지 아닌지의 여부입니다.
	 * @return 동적으로 생성한 이미지를 리턴합니다.
	 */
	UImage* CreatePlayerImage(const ETeam& NewTeam, bool bMyPlayer = false);

	FORCEINLINE const ETeam& GetTeam() const { return CurrentTeam; }

	FORCEINLINE void SetTeam(const ETeam& Team) { CurrentTeam = Team; }
private:
	/**
	 * @brief 월드를 미니맵 좌표로 변환합니다.
	 * @param PlayerLocation 월드 상에 존재하는 플레이어의 위치입니다.
	 * @param MiniMapSize 미니맵의 사이즈입니다.
	 * @return 미니맵 좌표계로 전환된 FVector2D를 리턴합니다.
	 */
	FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize);

	/**
	 * @brief 플레이어들의 위치를 미니맵 상에서 업데이트 해줍니다.
	 * @param Team 업데이트를 할 팀입니다.
	 */
	void UpdatePlayerPosition(const ETeam& Team);
	
public:
	// 미니맵을 업데이트 여부입니다.
	bool UpdateMinimap;

	// 미니맵에서 Anti팀과 Pro팀의 위치에 따라 이미지를 업데이트 해줘야 합니다.
	// 이를 관리 해주는 TMap입니다.
	TMap<ETeam, TMap<TWeakObjectPtr<ALakayaBasePlayerState>, TWeakObjectPtr<UImage>>> PlayersByMinimap;


private:
	TWeakObjectPtr<UCanvasPanel> CanvasPanel;
	
	TWeakObjectPtr<UImage> MinimapImage;

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

	// Onwer의 소속 팀 입니다.
	ETeam CurrentTeam;
};