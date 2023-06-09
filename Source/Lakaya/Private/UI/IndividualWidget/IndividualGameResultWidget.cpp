#include "UI/IndividualWidget/IndividualGameResultWidget.h"

void UIndividualGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Second_Rank_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("Second_Rank_Image is null."));
	if (Third_Rank_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("Third_Rank_Image is null."));
	if (Fourth_Rank_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("Fourth_Rank_Image is null."));
	if (Fifth_Rank_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("Fifth_Rank_Image is null."));
	if (Sixth_Rank_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("Sixth_Rank_Image is null."));
	
	if (Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("Score_Text is null."));
	if (Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("Kill_Text is null."));
	if (Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("Death_Text is null."));
}

void UIndividualGameResultWidget::SetScore(const uint16& ScoreCount) const
{
	if (Score_Text.IsValid()) Score_Text->SetText(FText::AsNumber(ScoreCount));
}

void UIndividualGameResultWidget::SetKill(const uint16& KillCount) const
{
	if (Kill_Text.IsValid()) Kill_Text->SetText(FText::AsNumber(KillCount));
}

void UIndividualGameResultWidget::SetDeath(const uint16& DeathCount) const
{
	if (Death_Text.IsValid()) Death_Text->SetText(FText::AsNumber(DeathCount));
}