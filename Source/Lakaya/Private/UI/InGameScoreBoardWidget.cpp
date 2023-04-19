#include "UI/InGameScoreBoardWidget.h"

#include "Character/CollectorPlayerState.h"

void UInGameScoreBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	// OccupationGameState = GetWorld() ? Cast<AOccupationGameState>(GetWorld()->GetGameState()) : nullptr;
	// if (OccupationGameState == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("InGameScoreBoardWidget_GameState is null."));
	// 	return;
	// }

	// for (int i = 0; i < OccupationGameState->GetNumPlayers(); i++)
	// {
	// 	APlayerState* PlayerState = OccupationGameState->PlayerArray.IsValidIndex(i) ? OccupationGameState->PlayerArray[i] : nullptr;
	// 	if (PlayerState == nullptr)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("PlayerState is null."));
	// 		continue;
	// 	}
	//
	// 	ACollectorPlayerState* InGameCollectorPlayerState = Cast<ACollectorPlayerState>(PlayerState);
	// 	if (InGameCollectorPlayerState == nullptr)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
	// 		continue;
	// 	}
	//
	// 	PlayerStatesArray.Add(InGameCollectorPlayerState);
	// }

#pragma region BindWidget
	A_Player_01 = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_01")));
	if (A_Player_01 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_01 is null."));
		return;
	}

	A_Player_02 = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_02")));
	if (A_Player_02 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_02 is null."));
		return;
	}

	A_Player_03 = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_03")));
	if (A_Player_03 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_03 is null."));
		return;
	}

	B_Player_01 = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_01")));
	if (B_Player_01 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_01 is null."));
		return;
	}

	B_Player_02 = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_02")));
	if (B_Player_02 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_02 is null."));
		return;
	}

	B_Player_03 = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_03")));
	if (B_Player_03 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_03 is null."));
		return;
	}

	A_Player_01_TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_01_TotalScore")));
	if (A_Player_01_TotalScore == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_01_TotalScore is null."));
		return;
	}

	A_Player_02_TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_02_TotalScore")));
	if (A_Player_02_TotalScore == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_02_TotalScore is null."));
		return;
	}

	A_Player_03_TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_03_TotalScore")));
	if (A_Player_03_TotalScore == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_03_TotalScore is null."));
		return;
	}

	B_Player_01_TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_01_TotalScore")));
	if (B_Player_01_TotalScore == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_01_TotalScore is null."));
		return;
	}

	B_Player_02_TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_02_TotalScore")));
	if (B_Player_02_TotalScore == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_02_TotalScore is null."));
		return;
	}

	B_Player_03_TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_03_TotalScore")));
	if (B_Player_03_TotalScore == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_03_TotalScore is null."));
		return;
	}

	A_Player_01_Kill = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_01_Kill")));
	if (A_Player_01_Kill == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_01_Kill is null."));
		return;
	}

	A_Player_02_Kill = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_02_Kill")));
	if (A_Player_02_Kill == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_02_Kill is null."));
		return;
	}

	A_Player_03_Kill = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_03_Kill")));
	if (A_Player_03_Kill == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_03_Kill is null."));
		return;
	}

	B_Player_01_Kill = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_01_Kill")));
	if (B_Player_01_Kill == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_01_Kill is null."));
		return;
	}

	B_Player_02_Kill = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_02_Kill")));
	if (B_Player_02_Kill == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_02_Kill is null."));
		return;
	}
	
	B_Player_03_Kill = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_03_Kill")));
	if (B_Player_03_Kill == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_03_Kill is null."));
		return;
	}

	A_Player_01_OcuSuc = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_01_OcuSuc")));
	if (A_Player_01_OcuSuc == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_01_OcuSuc is null."));
		return;
	}

	A_Player_02_OcuSuc = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_02_OcuSuc")));
	if (A_Player_02_OcuSuc == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_02_OcuSuc is null."));
		return;
	}

	A_Player_03_OcuSuc = Cast<UTextBlock>(GetWidgetFromName(TEXT("A_Player_03_OcuSuc")));
	if (A_Player_03_OcuSuc == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Player_03_OcuSuc is null."));
		return;
	}

	B_Player_01_OcuSuc = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_01_OcuSuc")));
	if (B_Player_01_OcuSuc == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_01_OcuSuc is null."));
		return;
	}

	B_Player_02_OcuSuc = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_02_OcuSuc")));
	if (B_Player_02_OcuSuc == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_02_OcuSuc is null."));
		return;
	}
	
	B_Player_03_OcuSuc = Cast<UTextBlock>(GetWidgetFromName(TEXT("B_Player_03_OcuSuc")));
	if (B_Player_03_OcuSuc == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("B_Player_03_OcuSuc is null."));
		return;
	}
#pragma endregion

	// SetVisibility(ESlateVisibility::Hidden);
}

void UInGameScoreBoardWidget::BindPlayerScore(ACollectorPlayerState* ArgCollectorPlayerState)
{
	// if (ArgCollectorPlayerState == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("ArgCollectorPlayerState is null."));
	// 	return;
	// }
	//
	// APlayerState* MyPlayerState = ArgCollectorPlayerState;
	// if (MyPlayerState == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("InGameScoreBoardWidget_MyPlayerState is null."));
	// 	return;
	// }

	// MyIndex = static_cast<int32>(OccupationGameState->GetPlayerIndex(MyPlayerState));
	ArgCollectorPlayerState->OnPlayerStateChange.AddUObject(this, &UInGameScoreBoardWidget::OnChangePlayerScore);
	// CollectorPlayerState = ArgCollectorPlayerState;
	UE_LOG(LogTemp, Warning, TEXT("Binding Success."));
}

void UInGameScoreBoardWidget::OnChangePlayerScore(FPlayerInfo NewPlayer, int32 NewScore, int32 NewKills, int32 NewOcuSuc,
	int32 NewOwnObjectNum)
{
	// A_Player_01_TotalScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.TotalScore)));
	// A_Player_01_Kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.Kills)));
	// A_Player_01_OcuSuc->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.OccupationSuccess)));
	//
	// B_Player_01_TotalScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.TotalScore)));
	// B_Player_01_Kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.Kills)));
	// B_Player_01_OcuSuc->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.OccupationSuccess)));
	//
	// A_Player_02_TotalScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.TotalScore)));
	// A_Player_02_Kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.Kills)));
	// A_Player_02_OcuSuc->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.OccupationSuccess)));
	//
	// B_Player_02_TotalScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.TotalScore)));
	// B_Player_02_Kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.Kills)));
	// B_Player_02_OcuSuc->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.OccupationSuccess)));
	//
	// A_Player_03_TotalScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.TotalScore)));
	// A_Player_03_Kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.Kills)));
	// A_Player_03_OcuSuc->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.OccupationSuccess)));
	//
	// B_Player_03_TotalScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.TotalScore)));
	// B_Player_03_Kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.Kills)));
	// B_Player_03_OcuSuc->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayer.OccupationSuccess)));
}