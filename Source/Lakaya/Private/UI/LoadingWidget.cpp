#include "UI/LoadingWidget.h"

#include "GameMode/OccupationGameState.h"


void ULoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AOccupationGameState* OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
		return;
	}

	OnChangeJoinedPlayers(OccupationGameState->GetNumPlayers(), OccupationGameState->GetMaxPlayers());

	// 바인딩
	LoadingWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadingWidgetText")));
	if (LoadingWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}

	OccupationGameState->OnOccupationChangeJoinedPlayers.AddUObject(this, &ULoadingWidget::OnChangeJoinedPlayers);
	OccupationGameState->OnOccupationChangeMaxPlayers.AddUObject(this, &ULoadingWidget::OnChangeMaxPlayers);
}

void ULoadingWidget::OnChangeJoinedPlayers(uint8 JoinedPlayers, uint8 MaxPlayer)
{
	if (JoinedPlayers == MaxPlayer)
	{
		LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("곧 게임을 시작합니다."))));
		return;
	}

	LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), JoinedPlayers, MaxPlayer)));
}

void ULoadingWidget::OnChangeMaxPlayers(uint8 Number, uint8 MaxPlayers)
{
	LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), Number, MaxPlayers)));
}
