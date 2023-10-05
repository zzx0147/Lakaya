#include "UI/GameResultWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"

void UGameResultWidget::ShowResult(const bool& IsWin, const float& AntiScore, const float& ProScore)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

	AntiScoreTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), AntiScore)));
	ProScoreTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), ProScore)));
	
	VictoryPanel->SetVisibility(IsWin ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	DefeatPanel->SetVisibility(IsWin ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible);
}