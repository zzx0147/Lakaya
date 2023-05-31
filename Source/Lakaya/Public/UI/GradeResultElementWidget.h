#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GradeResultElementWidget.generated.h"

UCLASS()
class LAKAYA_API UGradeResultElementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	TWeakObjectPtr<UImage> F_AntiWaziImage;
	TWeakObjectPtr<UImage> F_AntiRenaImage;
	TWeakObjectPtr<UImage> F_ProWaziImage;
	TWeakObjectPtr<UImage> F_ProRenaImage;
	TWeakObjectPtr<UImage> F_AntiRankBoardImage;
	TWeakObjectPtr<UImage> F_ProRankBoardImage;
	TWeakObjectPtr<UImage> F_ScoreBoardIconImage;
	
	TWeakObjectPtr<UTextBlock> F_RankText;
	TWeakObjectPtr<UTextBlock> F_NameText;
	TWeakObjectPtr<UTextBlock> F_TotalScoreText;

	TWeakObjectPtr<UImage> S_AntiWaziImage;
	TWeakObjectPtr<UImage> S_AntiRenaImage;
	TWeakObjectPtr<UImage> S_ProWaziImage;
	TWeakObjectPtr<UImage> S_ProRenaImage;
	TWeakObjectPtr<UImage> S_AntiRankBoardImage;
	TWeakObjectPtr<UImage> S_ProRankBoardImage;
	TWeakObjectPtr<UImage> S_ScoreBoardIconImage;
	
	TWeakObjectPtr<UTextBlock> S_RankText;
	TWeakObjectPtr<UTextBlock> S_NameText;
	TWeakObjectPtr<UTextBlock> S_TotalScoreText;

	TWeakObjectPtr<UImage> T_AntiWaziImage;
	TWeakObjectPtr<UImage> T_AntiRenaImage;
	TWeakObjectPtr<UImage> T_ProWaziImage;
	TWeakObjectPtr<UImage> T_ProRenaImage;
	TWeakObjectPtr<UImage> T_AntiRankBoardImage;
	TWeakObjectPtr<UImage> T_ProRankBoardImage;
	TWeakObjectPtr<UImage> T_ScoreBoardIconImage;
	
	TWeakObjectPtr<UTextBlock> T_RankText;
	TWeakObjectPtr<UTextBlock> T_NameText;
	TWeakObjectPtr<UTextBlock> T_TotalScoreText;
};