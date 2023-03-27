#define DO_CHECK 1

#include "UI/ScoreBoardElement.h"
#include "Components/TextBlock.h"

UScoreBoardElement::UScoreBoardElement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UScoreBoardElement::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	RankText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Rank_Text")));
	PlayerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName_Text")));
	ScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Score_Text")));

	check(RankText != nullptr);
	check(PlayerNameText != nullptr);
	check(ScoreText != nullptr);

#pragma endregion

}

void UScoreBoardElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UScoreBoardElement::SetData(int8 Rank, const FText PlayerName, int8 Score)
{
	RankText->SetText(FText::AsNumber(Rank));
	PlayerNameText->SetText(PlayerName);
	ScoreText->SetText(FText::AsNumber(Score));
}