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
};