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
	TWeakObjectPtr<UImage> AntiWaziImage;
	TWeakObjectPtr<UImage> AntiRenaImage;
	TWeakObjectPtr<UImage> ProWaziImage;
	TWeakObjectPtr<UImage> ProRenaImage;
	TWeakObjectPtr<UImage> AntiRankBoardImage;
	TWeakObjectPtr<UImage> ProRankBoardImage;

	TWeakObjectPtr<UTextBlock> RankText;
	TWeakObjectPtr<UTextBlock> NameText;
	TWeakObjectPtr<UTextBlock> TotalScoreText;
};