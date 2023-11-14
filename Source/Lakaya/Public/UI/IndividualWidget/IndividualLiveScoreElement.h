// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ScoreBoardElement.h"
#include "IndividualLiveScoreElement.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;

UCLASS()
class LAKAYA_API UIndividualLiveScoreElement : public UScoreBoardElement
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 1등의 Element 박스 이미지를 설정합니다.
	void Set1stElementBoxImage();

	// 2~6등의 Element 박스 이미지를 설정합니다.
	void SetOtherElementBoxImage();

	void SetPlayerCheckVisible();
	void SetPlayerCheckHidden();
	
	// 개인전 등수를 설정합니다.
	void SetIndividualRank(int32 RankCount);
	
	// 플레이어 이름을 설정합니다.
	virtual void SetPlayerName(const FText& PlayerName) const override;
	virtual void SetPlayerName(const FString& PlayerName) const override;

	// 누적 킬 횟수를 업데이트합니다.
	virtual void SetKillCount(const uint16& KillCount) const override;
	const uint16& GetKillCount() const { return IndividualKillCount; }

	mutable uint16 IndividualKillCount;

	TWeakObjectPtr<UImage> RankBox_Img;
	TWeakObjectPtr<UImage> PlayerCheck_Img;
private:
	TWeakObjectPtr<UTextBlock> RankText;
	TWeakObjectPtr<UTextBlock>  KillText;
	TWeakObjectPtr<UTextBlock>  NameText;
	
};
