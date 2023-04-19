#include "UI/GameResultWidget.h"
#include "Character/CollectorPlayerState.h"
#include "GameFramework/Character.h"

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
		return;
	}

	GameResultWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameResultWidgetText")));
	if (GameResultWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}

	OccupationGameState->OnOccupationChangeOccupationWinner.AddUObject(this, &UGameResultWidget::OnChangeWinner);

	SetVisibility(ESlateVisibility::Hidden);
}

void UGameResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGameResultWidget::ReMoveLoadingWidget(EOccupationGameState ChangeGameState)
{
	// TODO :
}

void UGameResultWidget::OnChangeWinner(EOccupationWinner NewWinner)
{
	SetVisibility(ESlateVisibility::Visible);

	ACollectorPlayerState* PlayerState = Cast<ACollectorPlayerState>(GetOwningPlayer()->GetCharacter()->GetPlayerState());
	if (PlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameResultWidget_PlayerState is null."));
		return;
	}
	
	FString WinnerString;
	switch (OccupationGameState->GetOccupationWinner())
	{
	case EOccupationWinner::UnCertain:
		WinnerString = FString(TEXT("Undecided"));
		break;
	case EOccupationWinner::A:
		if (PlayerState->GetPlayerTeamState() == EPlayerTeamState::A) GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("승리!"))));
		else GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("패배."))));
		break;
	case EOccupationWinner::B:
		if (PlayerState->GetPlayerTeamState() == EPlayerTeamState::A) GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("패배."))));
		else GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("승리!"))));
		break;
	default:
		WinnerString = FString(TEXT("Unknown"));
		break;
	}
}