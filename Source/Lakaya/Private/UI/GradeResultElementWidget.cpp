#include "UI/GradeResultElementWidget.h"

void UGradeResultElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AntiWaziImage = Cast<UImage>(GetWidgetFromName("Anti_Wazi_Rank_Image"));
	AntiRenaImage = Cast<UImage>(GetWidgetFromName("Anti_Rena_Rank_Image"));
	ProWaziImage = Cast<UImage>(GetWidgetFromName("Pro_Wazi_Rank_Image"));
	ProRenaImage = Cast<UImage>(GetWidgetFromName("Pro_Rena_Rank_Image"));
	AntiRankBoardImage = Cast<UImage>(GetWidgetFromName("Anti_RankBoard_Image"));
	ProRankBoardImage = Cast<UImage>(GetWidgetFromName("Pro_RankBoard_Image"));

	RankText = Cast<UTextBlock>(GetWidgetFromName("Rank_Text"));
	NameText = Cast<UTextBlock>(GetWidgetFromName("Name_Text"));
	TotalScoreText = Cast<UTextBlock>(GetWidgetFromName("TotalScore_Text"));

	if (AntiWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiWaziImage is null."));
	if (AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiRenaImage is null."));
	if (ProWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProWaziImage is null."));
	if (ProRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProRenaImage is null."));
	if (AntiRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiRankBoardImage is null."));
	if (ProRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProRankBoardImage is null."));
	if (RankText == nullptr) UE_LOG(LogTemp, Warning, TEXT("RankText is null."));
	if (NameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("NameText is null."));
	if (TotalScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("TotalScoreText is null."));
}