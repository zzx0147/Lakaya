#include "UI/GradeResultElementWidget.h"

void UGradeResultElementWidget::NativeConstruct()
{
	Super::NativeConstruct();
#pragma region 1st Widget Binding
	F_AntiWaziImage = Cast<UImage>(GetWidgetFromName("First_Anti_Wazi_Image"));
	F_AntiRenaImage = Cast<UImage>(GetWidgetFromName("First_Anti_Rena_Image"));
	F_ProWaziImage = Cast<UImage>(GetWidgetFromName("First_Pro_Wazi_Image"));
	F_ProRenaImage = Cast<UImage>(GetWidgetFromName("First_Pro_Rena_Image"));
	F_AntiRankBoardImage = Cast<UImage>(GetWidgetFromName("First_Anti_RankBoard_Image"));
	F_ProRankBoardImage = Cast<UImage>(GetWidgetFromName("First_Pro_RankBoard_Image"));
	F_ScoreBoardIconImage = Cast<UImage>(GetWidgetFromName("First_Scoreboard_Icon_Image"));

	F_RankText = Cast<UTextBlock>(GetWidgetFromName("First_Rank_Text"));
	F_NameText = Cast<UTextBlock>(GetWidgetFromName("First_Name_Text"));
	F_TotalScoreText = Cast<UTextBlock>(GetWidgetFromName("First_TotalScore_Text"));
#pragma endregion

#pragma region 2nd Widget Binding
	S_AntiWaziImage = Cast<UImage>(GetWidgetFromName("Second_Anti_Wazi_Image"));
	S_AntiRenaImage = Cast<UImage>(GetWidgetFromName("Second_Anti_Rena_Image"));
	S_ProWaziImage = Cast<UImage>(GetWidgetFromName("Second_Pro_Wazi_Image"));
	S_ProRenaImage = Cast<UImage>(GetWidgetFromName("Second_Pro_Rena_Image"));
	S_AntiRankBoardImage = Cast<UImage>(GetWidgetFromName("Second_Anti_RankBoard_Image"));
	S_ProRankBoardImage = Cast<UImage>(GetWidgetFromName("Second_Pro_RankBoard_Image"));
	S_ScoreBoardIconImage = Cast<UImage>(GetWidgetFromName("Second_Scoreboard_Icon_Image"));

	S_RankText = Cast<UTextBlock>(GetWidgetFromName("Second_Rank_Text"));
	S_NameText = Cast<UTextBlock>(GetWidgetFromName("Second_Name_Text"));
	S_TotalScoreText = Cast<UTextBlock>(GetWidgetFromName("Second_TotalScore_Text"));
#pragma endregion 

#pragma region 3rd Widget Binding
	T_AntiWaziImage = Cast<UImage>(GetWidgetFromName("Third_Anti_Wazi_Image"));
	T_AntiRenaImage = Cast<UImage>(GetWidgetFromName("Third_Anti_Rena_Image"));
	T_ProWaziImage = Cast<UImage>(GetWidgetFromName("Third_Pro_Wazi_Image"));
	T_ProRenaImage = Cast<UImage>(GetWidgetFromName("Third_Pro_Rena_Image"));
	T_AntiRankBoardImage = Cast<UImage>(GetWidgetFromName("Third_Anti_RankBoard_Image"));
	T_ProRankBoardImage = Cast<UImage>(GetWidgetFromName("Third_Pro_RankBoard_Image"));
	T_ScoreBoardIconImage = Cast<UImage>(GetWidgetFromName("Third_Scoreboard_Icon_Image"));

	T_RankText = Cast<UTextBlock>(GetWidgetFromName("Third_Rank_Text"));
	T_NameText = Cast<UTextBlock>(GetWidgetFromName("Third_Name_Text"));
	T_TotalScoreText = Cast<UTextBlock>(GetWidgetFromName("Third_TotalScore_Text"));
#pragma endregion

#pragma region Null Check
	if (F_AntiWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Anti_Wazi_Image is null."));
	if (F_AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Anti_Rena_Image is null."));
	if (F_ProWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Pro_Wazi_Image is null."));
	if (F_ProRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Pro_Rena_Image is null."));
	if (F_AntiRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Anti_RankBoard_Image is null."));
	if (F_ProRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Pro_RankBoard_Image is null."));
	if (F_RankText == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Scoreboard_Icon_Image is null."));
	if (F_NameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Rank_Text is null."));
	if (F_TotalScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_Name_Text is null."));
	if (F_ScoreBoardIconImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_TotalScore_Text is null."));

	if (S_AntiWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Anti_Wazi_Image is null."));
	if (S_AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Anti_Rena_Image is null."));
	if (S_ProWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Pro_Wazi_Image is null."));
	if (S_ProRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Pro_Rena_Image is null."));
	if (S_AntiRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Anti_RankBoard_Image is null."));
	if (S_ProRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Pro_RankBoard_Image is null."));
	if (S_RankText == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Scoreboard_Icon_Image is null."));
	if (S_NameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Rank_Text is null."));
	if (S_TotalScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Name_Text is null."));
	if (S_ScoreBoardIconImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("First_TotalScore_Text is null."));

	if (T_AntiWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Anti_Wazi_Image is null."));
	if (T_AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Anti_Rena_Image is null."));
	if (T_ProWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Pro_Wazi_Image is null."));
	if (T_ProRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Pro_Rena_Image is null."));
	if (T_AntiRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Anti_RankBoard_Image is null."));
	if (T_ProRankBoardImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Pro_RankBoard_Image is null."));
	if (T_RankText == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Scoreboard_Icon_Image is null."));
	if (T_NameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Rank_Text is null."));
	if (T_TotalScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Name_Text is null."));
	if (T_ScoreBoardIconImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_TotalScore_Text is null."));
#pragma endregion
}