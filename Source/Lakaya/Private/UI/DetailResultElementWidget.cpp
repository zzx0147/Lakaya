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
	ESlateVisibility VisibilityType = ESlateVisibility::SelfHitTestInvisible;
	bool bIsTeamA = (NewPlayerState->GetTeam() == ETeam::Anti);
	bool bIsTeamB = (NewPlayerState->GetTeam() == ETeam::Pro);
	bool bIsRena = (NewPlayerState->GetCharacterName() == TEXT("Rena"));
	bool bIsWazi = (NewPlayerState->GetCharacterName() == TEXT("Wazi"));

	if (bIsTeamA)
	{
		Panels = {AF_Panel, AS_Panel, AT_Panel};
		Portrait_Rena_Images = {AF_Portrait_Rena_Image, AS_Portrait_Rena_Image, AT_Portrait_Rena_Image};
		Portrait_Wazi_Images = {AF_Portrait_Wazi_Image, AS_Portrait_Wazi_Image, AT_Portrait_Wazi_Image};
		PlayerName_Texts = {AF_PlayerName_Text, AS_PlayerName_Text, AT_PlayerName_Text};
		Score_Texts = {AF_Score_Text, AS_Score_Text, AT_Score_Text};
		Occupation_Texts = {AF_Occupation_Text, AS_Occupation_Text, AT_Occupation_Text};
		Kill_Texts = {AF_Kill_Text, AS_Kill_Text, AT_Kill_Text};
		Death_Texts = {AF_Death_Text, AS_Death_Text, AT_Death_Text};
	}
	else if (bIsTeamB)
	{
		Panels = {PF_Panel, PS_Panel, PT_Panel};
		Portrait_Rena_Images = {PF_Portrait_Rena_Image, PS_Portrait_Rena_Image, PT_Portrait_Rena_Image};
		Portrait_Wazi_Images = {PF_Portrait_Wazi_Image, PS_Portrait_Wazi_Image, PT_Portrait_Wazi_Image};
		PlayerName_Texts = {PF_PlayerName_Text, PS_PlayerName_Text, PT_PlayerName_Text};
		Score_Texts = {PF_Score_Text, PS_Score_Text, PT_Score_Text};
		Occupation_Texts = {PF_Occupation_Text, PS_Occupation_Text, PT_Occupation_Text};
		Kill_Texts = {PF_Kill_Text, PS_Kill_Text, PT_Kill_Text};
		Death_Texts = {PF_Death_Text, PS_Death_Text, PT_Death_Text};
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Team"));
		return;
	}

	if (Index < Panels.Num())
	{
		Panels[Index]->SetVisibility(VisibilityType);
		Portrait_Rena_Images[Index]->SetVisibility(bIsRena ? VisibilityType : ESlateVisibility::Collapsed);
		Portrait_Wazi_Images[Index]->SetVisibility(bIsWazi ? VisibilityType : ESlateVisibility::Collapsed);
		SetPlayerName(PlayerName_Texts[Index].Get(), *NewPlayerState->GetPlayerName());
		SetTotalScore(Score_Texts[Index].Get(), NewPlayerState->GetTotalScore());
		SetSuccessCaptureCount(Occupation_Texts[Index].Get(), NewPlayerState->GetSuccessCaptureCount());
		SetKillCount(Kill_Texts[Index].Get(), NewPlayerState->GetKillCount());
		SetDeathCount(Death_Texts[Index].Get(), NewPlayerState->GetDeathCount());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Index Out Of Range"));
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