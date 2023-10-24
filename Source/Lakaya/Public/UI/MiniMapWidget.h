// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Occupation/Team.h"
#include "MiniMapWidget.generated.h"

class ALakayaBaseCharacter;

/**
 * 
 */
UCLASS()
class LAKAYA_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 특정 적의 위치를 업데이트합니다.
	 * @param NewTeam 위치를 업데이트 할 플레이어의 팀입니다.
	 * @param NewPlayerState 위치를 업데이트 할 플레이어의 상태입니다.
	 */
	virtual void UpdatePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) { return; }

	/**
	 * @brief 특정 적의 위치를 숨깁니다.
	 * @param NewTeam 
	 * @param NewPlayerState 
	 */
	virtual void HidePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) { return; }
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/**
	* @brief 미니맵에 모든 플레이어들의 아이콘(이미지)으로 띄우기 위해, 플레이어 인원 수만큼 이미지를 동적 생성합니다.
	* @param NewTeam 팀에 따라, 아이콘의 이미지가 달라지는데, 생성된 팀의 정보입니다.
	* @param bMyPlayer 나 자신인지 아닌지의 여부입니다.
	* @return 동적으로 생성한 이미지를 리턴합니다.
	*/
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer = false) { return nullptr; }

	/**
	* @brief 월드를 미니맵 좌표로 변환합니다.
	* @param PlayerLocation 월드 상에 존재하는 플레이어의 위치입니다.
	* @param MiniMapSize 미니맵의 사이즈입니다.
	* @return 미니맵 좌표계로 전환된 FVector2D를 리턴합니다.
	*/
	virtual FVector2D ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation, const FVector2D& MiniMapSize);

	/**
	 * @brief 플레이어팀의 위치를 업데이트합니다.
	 * @param Team 플레이어의 팀입니다.
	 */
	virtual void UpdatePlayerPosition(const ETeam& Team);
public:
	FORCEINLINE const ETeam& GetTeam() const { return CurrentTeam; }
	FORCEINLINE const bool& GetAllyUpdateMinimap() const { return AllyUpdateMinimap; }
	FORCEINLINE const bool& GetEnemyUpdateMinimap() const { return EnemyUpdateMinimap; }
	// FORCEINLINE const TObjectPtr<ALakayaBaseCharacter> GetOwnerCharacter() const { return OwnerCharacter; }
	FORCEINLINE const TMap<ETeam, TMap<TWeakObjectPtr<ALakayaBasePlayerState>, TWeakObjectPtr<UImage>>>& GetPlayersByMinimap() const { return PlayersByMinimap; }
	
	FORCEINLINE void SetTeam(const ETeam& Team) { CurrentTeam = Team; }
	FORCEINLINE void SetAllyUpdateMinimap(const bool& bUpdate) { AllyUpdateMinimap = bUpdate; }
	FORCEINLINE void SetEnemyUpdateMinimap(const bool& bUpdate) { EnemyUpdateMinimap = bUpdate; }
	// FORCEINLINE void SetOwnerCharacter(const TObjectPtr<ALakayaBaseCharacter> NewCharacter) { OwnerCharacter = NewCharacter; }
	FORCEINLINE void SetPlayersByMinimap(const ETeam& Team, ALakayaBasePlayerState* NewPlayerState, UImage* NewImage) { if (NewImage != nullptr) PlayersByMinimap[Team].Emplace(NewPlayerState, NewImage); }

protected:
	// Onwer의 Character입니다.
	// UPROPERTY()
	// TObjectPtr<ALakayaBaseCharacter> OwnerCharacter;
	
	// 위젯의 최상단 CanvasPanel
	UPROPERTY()
	TObjectPtr<UCanvasPanel> ParentPanel;

	// 미니맵 이미지입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MinimapImage;

	// 미니맵상에 자기 자신(개인전)을 표시하는 아이콘 텍스쳐입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> IndividualOwnIcon;

	// 미니맵상에 적(개인전)을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> IndividualIcon;
	
	// 미니맵상에 자기 자신(Anti팀)을 표시하는 아이콘 텍스쳐입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiOwnIcon;

	// 미니맵상에 자기 자신(Pro팀)을 표시하는 아이콘 텍스쳐입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProOwnIcon;

	// 미니맵상에 Anti팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AntiIcon;

	// 미니맵상에 Pro팀을 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ProIcon;
	
	// 플레이어가 죽게 되었을 때, 미니맵 상에 표시하는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> DeathIcon;

	// 적이 시야에서 사라지게 되었을 때, 미니맵 상에 표시되는 아이콘 텍스처입니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> QuestionMarkIcon;
	
	// Owner의 소속 팀 입니다.
	ETeam CurrentTeam;

	// Owner의 상대 팀 입니다.
	ETeam EnemyTeam;
	
	// 미니맵 상에 표시될 Icon들의 Alignment 입니다.
	FVector2D IconAlignment;

	// 미니맵 상에 표시될 Icon들의 Size 입니다.
	FVector2D IconSize;

	// 미니맵 이미지의 Size 입니다.
	FVector2D MinimapSize;

	// 미니맵이 위치한 Widget의 Offset 입니다.
	FVector2D WidgetOffset;

	// 아군의 미니맵업데이트 여부입니다.
	bool AllyUpdateMinimap;

	// 적군의 미니맵업데이트 여부입니다.
	bool EnemyUpdateMinimap;

	// 미니맵상에서 자신과 상대(AI포함)의 위치를 업데이트하기 위한 컨테이너입니다.
	TMap<ETeam, TMap<TWeakObjectPtr<ALakayaBasePlayerState>, TWeakObjectPtr<UImage>>> PlayersByMinimap;

	// AntiIcon, ProIcon을 담는 맵입니다.
	UPROPERTY()
	TMap<ETeam, TObjectPtr<UTexture2D>> TeamIcons;
	
	// TODO :
	FTimerHandle QuestionIconTimerHandle;
};