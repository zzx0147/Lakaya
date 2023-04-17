#include "UI/LoadingWidget.h"

#include "GameMode/OccupationGameState.h"


void ULoadingWidget::OnBeginPlay()
{
	Super::OnBeginPlay();
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void ULoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LoadingWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadingWidgetText")));
	if (LoadingWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}

	const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
		return;
	}

	MaxPlayerCount = OccupationGameState->GetMaxPlayers();
	OnChangeJoinedPlayers(OccupationGameState->GetNumPlayers());

	OccupationGameState->OnOccupationChangeJoinedPlayers.AddUObject(this, &ULoadingWidget::OnChangeJoinedPlayers);
}

void ULoadingWidget::OnChangeJoinedPlayers(const uint8& PlayerCount) const
{
	if (PlayerCount == MaxPlayerCount)
	{
		LoadingWidgetText->SetText(FText::FromString(TEXT("곧 게임을 시작합니다.")));
		return;
	}

	LoadingWidgetText->SetText(
		FText::FromString(FString::Printf(TEXT("(%d / %d)"), PlayerCount, MaxPlayerCount)));
}
