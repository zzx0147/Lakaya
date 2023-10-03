// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Occupation/Team.h"
#include "TabMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UTabMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/**
	 * @brief 월드를 미니맵 좌표로 변환합니다.
	 * @param PlayerLocation 월드 상에 존재하는 플레이어의 위치입니다.
	 * @param MiniMapSize 미니맵의 사이즈입니다.
	 * @return 미니맵 좌표계로 전환된 FVector2D를 리턴합니다.
	 */
	virtual FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize);

	/**
	 * @brief 플레이어들의 위치를 미니맵 상에서 업데이트 해줍니다.
	 * @param Team 업데이트를 할 팀입니다.
	 */
	virtual void UpdatePlayerPosition(const ETeam& Team);

	/**
	* @brief 미니맵에 모든 플레이어들의 아이콘(이미지)으로 띄우기 위해, 플레이어 인원 수만큼 이미지를 동적 생성합니다.
	* @param NewTeam 팀에 따라, 아이콘의 이미지가 달라지는데, 생성된 팀의 정보입니다.
	* @param bMyPlayer 나 자신인지 아닌지의 여부입니다.
	* @return 동적으로 생성한 이미지를 리턴합니다.
	*/
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) { return nullptr; };
public:

	FORCEINLINE const ETeam& GetTeam() const { return CurrentTeam; }

	FORCEINLINE void SetTeam(const ETeam& Team) { CurrentTeam = Team; }

protected:
	// TODO : 팀전과 개인전 각각의 최상단의 CanvasPanel를 만들어서 진행해야 합니다.
	// 위젯의 최상단 CanvasPanel
	// UPROPERTY()
	// TObjectPtr<UCanvasPanel> CanvasPanel;

	// 미니맵이미지를 담을 이미지
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MinimapImage;

	// TODO : IndividualTabMinimapWidget으로 옮겨줘야 합니다.
	// 미니맵상에 적팀을 표시하는 아이콘 텍스쳐입니다.
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UTexture2D> EnemyIcon;

	// 미니맵상에 자기 자신을 표시하는 아이콘 텍스쳐입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> OwnIcon;

	// 플레이어가 죽게 되었을 때, 미니맵 상에 표시할 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> DeathIcon;

	// Owner의 소속 팀 입니다.
	ETeam CurrentTeam;

	FVector2D IconAlignment;
	FVector2D IconSize;
	
public:
	// 미니맵업데이트 여부입니다.
	bool UpdateMinimap;

	// 미니맵상에서 자신과 상대(AI포함)의 위치를 업데이트하기 위한 컨테이너입니다.
	TMap<ETeam, TMap<TWeakObjectPtr<ALakayaBasePlayerState>, TWeakObjectPtr<UImage>>> PlayersByMinimap;
};