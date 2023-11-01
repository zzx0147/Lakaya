// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ScoreBoardElement.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "AIIndividualScoreBoardElement.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UAIIndividualScoreBoardElement : public UScoreBoardElement
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:

	// // 개인전 등수를 설정합니다.
	// void SetIndividualRank(const uint16& RankCount) const;
	
	// 플레이어 이름을 설정합니다.
	virtual void SetPlayerName(const FText& PlayerName) const override;
	virtual void SetPlayerName(const FString& PlayerName) const override;

	// 누적 킬 횟수를 업데이트합니다.
	virtual void SetKillCount(const uint16& KillCount) const override;

	// 누적 사망 횟수를 업데이트합니다.
	virtual void SetDeathCount(const uint16& DeathCount) const override;


private:
	TWeakObjectPtr<UTextBlock> PlayerNameText;	 			// 플레이어 이름
	TWeakObjectPtr<UTextBlock> ScoreText;	   	 			// 점수
	TWeakObjectPtr<UTextBlock> CaptureText;		 			// 점령 성공
	TWeakObjectPtr<UTextBlock> KillText;		 			// 킬
	TWeakObjectPtr<UTextBlock> DeathText;					// 데스
	
	// TWeakObjectPtr<UTextBlock> IndividualRankText;			// 개인전 등수
	TWeakObjectPtr<UTextBlock> IndividualPlayerNameText;	// 개인전 플레이어 이름
	TWeakObjectPtr<UTextBlock> IndividualKillText;			// 개인전 킬
	TWeakObjectPtr<UTextBlock> IndividualDeathText;			// 개인전 데스

public:
	TWeakObjectPtr<UImage> Anti_BackGround_Image;			// Anti_뒷배경
	TWeakObjectPtr<UImage> Pro_BackGround_Image;			// Pro_뒷배경
	TWeakObjectPtr<UImage> Character_Rena_Image;			// Anti_레나 초상화
	TWeakObjectPtr<UImage> Character_Wazi_Image;			// Anti_와지 초상화
	
	TWeakObjectPtr<UImage> Individual_BackGround_Image;		// Individual_뒷배경
};
