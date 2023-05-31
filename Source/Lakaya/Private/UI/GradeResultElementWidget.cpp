#include "UI/GradeResultElementWidget.h"

void UGradeResultElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	F_AntiWaziImage = Cast<UImage>(GetWidgetFromName("1_Anti_Wazi_Rank_Image"));
	F_AntiRenaImage = Cast<UImage>(GetWidgetFromName("1_Anti_Rena_Rank_Image"));
	F_ProWaziImage = Cast<UImage>(GetWidgetFromName("1_Pro_Wazi_Rank_Image"));
	F_ProRenaImage = Cast<UImage>(GetWidgetFromName("1_Pro_Rena_Rank_Image"));
	F_AntiRankBoardImage = Cast<UImage>(GetWidgetFromName("1_Anti_RankBoard_Image"));
	F_ProRankBoardImage = Cast<UImage>(GetWidgetFromName("1_Pro_RankBoard_Image"));
	F_ScoreBoardIconImage = Cast<UImage>(GetWidgetFromName("1_Scoreboard_Icon_Image"));

	F_RankText = Cast<UTextBlock>(GetWidgetFromName("1_Rank_Text"));
	F_NameText = Cast<UTextBlock>(GetWidgetFromName("1_Name_Text"));
	F_TotalScoreText = Cast<UTextBlock>(GetWidgetFromName("1_TotalScore_Text"));

	if (F_AntiWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_AntiWaziImage is null."));
	if (F_AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_AntiRenaImage is null."));
	if (F_ProWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_ProWaziImage is null."));
	if (F_ProRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_ProRenaImage is null."));
	if (F_AntiRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_AntiRankBoardImage is null."));
	if (F_ProRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_ProRankBoardImage is null."));
	if (F_RankText == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_RankText is null."));
	if (F_NameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_NameText is null."));
	if (F_TotalScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_TotalScoreText is null."));
	if (F_ScoreBoardIconImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("1_ScoreBoardIconImage is null."));
}