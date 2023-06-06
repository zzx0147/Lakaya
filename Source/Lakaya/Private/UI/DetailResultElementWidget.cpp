#include "UI/DetailResultElementWidget.h"

void UDetailResultElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region Initializer
	AF_Panel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("AF_Panel")));
	AS_Panel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("AS_Panel")));
	AT_Panel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("AT_Panel")));
	PF_Panel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PF_Panel")));
	PS_Panel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PS_Panel")));
	PT_Panel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PT_Panel")));

	AF_Portrait_Rena_Image = Cast<UImage>(GetWidgetFromName(TEXT("AF_Portrait_Rena_Image")));
	AF_Portrait_Wazi_Image = Cast<UImage>(GetWidgetFromName(TEXT("AF_Portrait_Wazi_Image")));
	AF_PlayerName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AF_PlayerName_Text")));
	AF_Score_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AF_Score_Text")));
	AF_Occupation_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AF_Occupation_Text")));
	AF_Kill_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AF_Kill_Text")));
	AF_Death_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AF_Death_Text")));

	AS_Portrait_Rena_Image = Cast<UImage>(GetWidgetFromName(TEXT("AS_Portrait_Rena_Image")));
	AS_Portrait_Wazi_Image = Cast<UImage>(GetWidgetFromName(TEXT("AS_Portrait_Wazi_Image")));
	AS_PlayerName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AS_PlayerName_Text")));
	AS_Score_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AS_Score_Text")));
	AS_Occupation_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AS_Occupation_Text")));
	AS_Kill_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AS_Kill_Text")));
	AS_Death_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AS_Death_Text")));

	AT_Portrait_Rena_Image = Cast<UImage>(GetWidgetFromName(TEXT("AT_Portrait_Rena_Image")));
	AT_Portrait_Wazi_Image = Cast<UImage>(GetWidgetFromName(TEXT("AT_Portrait_Wazi_Image")));
	AT_PlayerName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AT_PlayerName_Text")));
	AT_Score_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AT_Score_Text")));
	AT_Occupation_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AT_Occupation_Text")));
	AT_Kill_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AT_Kill_Text")));
	AT_Death_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("AT_Death_Text")));
	
	PF_Portrait_Rena_Image = Cast<UImage>(GetWidgetFromName(TEXT("PF_Portrait_Rena_Image")));
	PF_Portrait_Wazi_Image = Cast<UImage>(GetWidgetFromName(TEXT("PF_Portrait_Wazi_Image")));
	PF_PlayerName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PF_PlayerName_Text")));
	PF_Score_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PF_Score_Text")));
	PF_Occupation_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PF_Occupation_Text")));
	PF_Kill_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PF_Kill_Text")));
	PF_Death_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PF_Death_Text")));

	PS_Portrait_Rena_Image = Cast<UImage>(GetWidgetFromName(TEXT("PS_Portrait_Rena_Image")));
	PS_Portrait_Wazi_Image = Cast<UImage>(GetWidgetFromName(TEXT("PS_Portrait_Wazi_Image")));
	PS_PlayerName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PS_PlayerName_Text")));
	PS_Score_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PS_Score_Text")));
	PS_Occupation_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PS_Occupation_Text")));
	PS_Kill_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PS_Kill_Text")));
	PS_Death_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PS_Death_Text")));

	PT_Portrait_Rena_Image = Cast<UImage>(GetWidgetFromName(TEXT("PT_Portrait_Rena_Image")));
	PT_Portrait_Wazi_Image = Cast<UImage>(GetWidgetFromName(TEXT("PT_Portrait_Wazi_Image")));
	PT_PlayerName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PT_PlayerName_Text")));
	PT_Score_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PT_Score_Text")));
	PT_Occupation_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PT_Occupation_Text")));
	PT_Kill_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PT_Kill_Text")));
	PT_Death_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("PT_Death_Text")));
#pragma endregion

#pragma region NullCheck
	if (AF_Panel == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Panel is null"));
	if (AS_Panel == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Panel is null"));
	if (AT_Panel == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Panel is null"));
	if (PF_Panel == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Panel is null"));
	if (PS_Panel == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Panel is null"));
	if (PT_Panel == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Panel is null"));

	if (AF_Portrait_Rena_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Portrait_Rena_Image is null."));
	if (AF_Portrait_Wazi_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Portrait_Wazi_Image is null."));
	if (AF_PlayerName_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_PlayerName_Text is null."));
	if (AF_Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Score_Text is null."));
	if (AF_Occupation_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Occupation_Text is null."));
	if (AF_Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Kill_Text is null."));
	if (AF_Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AF_Death_Text is null."));

	if (AS_Portrait_Rena_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Portrait_Rena_Image is null."));
	if (AS_Portrait_Wazi_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Portrait_Wazi_Image is null."));
	if (AS_PlayerName_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_PlayerName_Text is null."));
	if (AS_Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Score_Text is null."));
	if (AS_Occupation_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Occupation_Text is null."));
	if (AS_Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Kill_Text is null."));
	if (AS_Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AS_Death_Text is null."));

	if (AT_Portrait_Rena_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Portrait_Rena_Image is null."));
	if (AT_Portrait_Wazi_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Portrait_Wazi_Image is null."));
	if (AT_PlayerName_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_PlayerName_Text is null."));
	if (AT_Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Score_Text is null."));
	if (AT_Occupation_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Occupation_Text is null."));
	if (AT_Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Kill_Text is null."));
	if (AT_Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("AT_Death_Text is null."));

	if (PF_Portrait_Rena_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Portrait_Rena_Image is null."));
	if (PF_Portrait_Wazi_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Portrait_Wazi_Image is null."));
	if (PF_PlayerName_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_PlayerName_Text is null."));
	if (PF_Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Score_Text is null."));
	if (PF_Occupation_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Occupation_Text is null."));
	if (PF_Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Kill_Text is null."));
	if (PF_Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PF_Death_Text is null."));

	if (PS_Portrait_Rena_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Portrait_Rena_Image is null."));
	if (PS_Portrait_Wazi_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Portrait_Wazi_Image is null."));
	if (PS_PlayerName_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_PlayerName_Text is null."));
	if (PS_Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Score_Text is null."));
	if (PS_Occupation_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Occupation_Text is null."));
	if (PS_Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Kill_Text is null."));
	if (PS_Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PS_Death_Text is null."));

	if (PT_Portrait_Rena_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Portrait_Rena_Image is null."));
	if (PT_Portrait_Wazi_Image == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Portrait_Wazi_Image is null."));
	if (PT_PlayerName_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_PlayerName_Text is null."));
	if (PT_Score_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Score_Text is null."));
	if (PT_Occupation_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Occupation_Text is null."));
	if (PT_Kill_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Kill_Text is null."));
	if (PT_Death_Text == nullptr) UE_LOG(LogTemp, Warning, TEXT("PT_Death_Text is null."));
#pragma endregion 
}

void UDetailResultElementWidget::SetElementWidget(const ALakayaBasePlayerState* NewPlayerState, const uint8 Index)
{
	if (NewPlayerState->GetTeam() == EPlayerTeam::A)
	{
		switch (Index)
		{
		case 0:
			AF_Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Rena")
				AF_Portrait_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Wazi")
				AF_Portrait_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SetPlayerName(AF_PlayerName_Text.Get(), *NewPlayerState->GetName());
			SetTotalScore(AF_Score_Text.Get(), NewPlayerState->GetTotalScore());
			SetSuccessCaptureCount(AF_Occupation_Text.Get(), NewPlayerState->GetSuccessCaptureCount());
			SetKillCount(AF_Kill_Text.Get(), NewPlayerState->GetKillCount());
			SetDeathCount(AF_Death_Text.Get(), NewPlayerState->GetDeathCount());
			break;
		case 1:
			AS_Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Rena")
				AS_Portrait_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Wazi")
				AS_Portrait_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SetPlayerName(AS_PlayerName_Text.Get(), *NewPlayerState->GetName());
			SetTotalScore(AS_Score_Text.Get(), NewPlayerState->GetTotalScore());
			SetSuccessCaptureCount(AS_Occupation_Text.Get(), NewPlayerState->GetSuccessCaptureCount());
			SetKillCount(AS_Kill_Text.Get(), NewPlayerState->GetKillCount());
			SetDeathCount(AS_Death_Text.Get(), NewPlayerState->GetDeathCount());
			break;
		case 2:
			AT_Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Rena")
				AT_Portrait_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Wazi")
				AT_Portrait_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SetPlayerName(AT_PlayerName_Text.Get(), *NewPlayerState->GetName());
			SetTotalScore(AT_Score_Text.Get(), NewPlayerState->GetTotalScore());
			SetSuccessCaptureCount(AT_Occupation_Text.Get(), NewPlayerState->GetSuccessCaptureCount());
			SetKillCount(AT_Kill_Text.Get(), NewPlayerState->GetKillCount());
			SetDeathCount(AT_Death_Text.Get(), NewPlayerState->GetDeathCount());
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Anti_Switch_Default"));
			break;
		}
	}

	if (NewPlayerState->GetTeam() == EPlayerTeam::B)
	{
		switch (Index)
		{
			case 0:
			PF_Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Rena")
				PF_Portrait_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Wazi")
				PF_Portrait_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SetPlayerName(PF_PlayerName_Text.Get(), *NewPlayerState->GetName());
			SetTotalScore(PF_Score_Text.Get(), NewPlayerState->GetTotalScore());
			SetSuccessCaptureCount(PF_Occupation_Text.Get(), NewPlayerState->GetSuccessCaptureCount());
			SetKillCount(PF_Kill_Text.Get(), NewPlayerState->GetKillCount());
			SetDeathCount(PF_Death_Text.Get(), NewPlayerState->GetDeathCount());
			break;
		case 1:
			PS_Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Rena")
				PS_Portrait_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Wazi")
				PS_Portrait_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SetPlayerName(PS_PlayerName_Text.Get(), *NewPlayerState->GetName());
			SetTotalScore(PS_Score_Text.Get(), NewPlayerState->GetTotalScore());
			SetSuccessCaptureCount(PS_Occupation_Text.Get(), NewPlayerState->GetSuccessCaptureCount());
			SetKillCount(PS_Kill_Text.Get(), NewPlayerState->GetKillCount());
			SetDeathCount(PS_Death_Text.Get(), NewPlayerState->GetDeathCount());
			break;
		case 2:
			PT_Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Rena")
				PT_Portrait_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (NewPlayerState->GetCharacterName() == "Wazi")
				PT_Portrait_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SetPlayerName(PT_PlayerName_Text.Get(), *NewPlayerState->GetName());
			SetTotalScore(PT_Score_Text.Get(), NewPlayerState->GetTotalScore());
			SetSuccessCaptureCount(PT_Occupation_Text.Get(), NewPlayerState->GetSuccessCaptureCount());
			SetKillCount(PT_Kill_Text.Get(), NewPlayerState->GetKillCount());
			SetDeathCount(PT_Death_Text.Get(), NewPlayerState->GetDeathCount());
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Pro_Switch_Default"));
			break;
		}
	}
}

void UDetailResultElementWidget::SetPlayerName(UTextBlock* NewTextBlock, const FText& PlayerName) const
{
	if (IsValid(NewTextBlock)) NewTextBlock->SetText(PlayerName);
}

void UDetailResultElementWidget::SetPlayerName(UTextBlock* NewTextBlock, const FString& PlayerName) const
{
	SetPlayerName(NewTextBlock, FText::FromString(PlayerName));
}

void UDetailResultElementWidget::SetTotalScore(UTextBlock* NewTextBlock, const uint16& ScoreCount) const
{
	if (IsValid(NewTextBlock)) NewTextBlock->SetText(FText::AsNumber(ScoreCount));
}

void UDetailResultElementWidget::SetSuccessCaptureCount(UTextBlock* NewTextBlock, const uint16& CaptureCount) const
{
	if (IsValid(NewTextBlock)) NewTextBlock->SetText(FText::AsNumber(CaptureCount));
}

void UDetailResultElementWidget::SetKillCount(UTextBlock* NewTextBlock, const uint16& KillCount) const
{
	if (IsValid(NewTextBlock)) NewTextBlock->SetText(FText::AsNumber(KillCount));
}

void UDetailResultElementWidget::SetDeathCount(UTextBlock* NewTextBlock, const uint16& DeathCount) const
{
	if (IsValid(NewTextBlock)) NewTextBlock->SetText(FText::AsNumber(DeathCount));
}