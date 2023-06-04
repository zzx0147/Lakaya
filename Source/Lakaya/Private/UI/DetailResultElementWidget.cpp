#include "UI/DetailResultElementWidget.h"

void UDetailResultElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AntiTotalBoxElementImage = Cast<UImage>(GetWidgetFromName(TEXT("TotalBoxElement_Anti_Image")));
	ProTotalBoxElementImage = Cast<UImage>(GetWidgetFromName(TEXT("TotalBoxElement_Pro_Image")));

	PlayerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName_Text")));

	PortraitRenaImage = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_Rena_Image")));
	PortraitWaziImage = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_Wazi_Image")));

	ScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Score_Text")));
	OccupationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Occupation_Text")));
	KillText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Kill_Text")));
	DeathText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Death_Text")));

	if (AntiTotalBoxElementImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiTotalBoxElementImage is null."));
	if (ProTotalBoxElementImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProTotalBoxelementImage is null."));
	if (PlayerNameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("PlayerNameText is null."));
	if (PortraitRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("PortraitRenaImage is null."));
	if (PortraitWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("PortraitWaziImage is null."));
	if (ScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("ScoreText is null."));
	if (OccupationText == nullptr) UE_LOG(LogTemp, Warning, TEXT("OccupationText is null."));
	if (KillText == nullptr) UE_LOG(LogTemp, Warning, TEXT("KillText is null."));
	if (DeathText == nullptr) UE_LOG(LogTemp, Warning, TEXT("DeathText is null."));
}

void UDetailResultElementWidget::SetPlayerName(const FText& PlayerName) const
{
	if (PlayerNameText.IsValid()) PlayerNameText->SetText(PlayerName);
}

void UDetailResultElementWidget::SetPlayerName(const FString& PlayerName) const
{
	SetPlayerName(FText::FromString(PlayerName));
}

void UDetailResultElementWidget::SetTotalScore(const uint16& ScoreCount) const
{
	if (ScoreText.IsValid()) ScoreText->SetText(FText::AsNumber(ScoreCount));
}

void UDetailResultElementWidget::SetSuccessCaptureCount(const uint16& CaptureCount) const
{
	if (OccupationText.IsValid()) OccupationText->SetText(FText::AsNumber(CaptureCount));
}

void UDetailResultElementWidget::SetKillCount(const uint16& KillCount) const
{
	if (KillText.IsValid()) KillText->SetText(FText::AsNumber(KillCount));
}

void UDetailResultElementWidget::SetDeathCount(const uint16& DeathCount) const
{
	if (DeathText.IsValid()) DeathText->SetText(FText::AsNumber(DeathCount));
}

void UDetailResultElementWidget::SetAntiTotalBoxElementImage() const
{
	if (AntiTotalBoxElementImage.IsValid()) AntiTotalBoxElementImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDetailResultElementWidget::SetProTotalBoxElementImage() const
{
	if (ProTotalBoxElementImage.IsValid()) ProTotalBoxElementImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDetailResultElementWidget::SetPortraitRenaImage() const
{
	if (PortraitRenaImage.IsValid()) PortraitRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDetailResultElementWidget::SetPortraitWaziImage() const
{
	if (PortraitWaziImage.IsValid()) PortraitWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}