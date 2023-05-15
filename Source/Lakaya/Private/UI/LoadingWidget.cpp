#include "UI/LoadingWidget.h"

void ULoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LoadingWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadingWidgetText")));
	if (LoadingWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}
}

void ULoadingWidget::SetPlayerNumber(const uint8& PlayerCount)
{
	if (PlayerCount == MaxPlayerCount)
	{
		LoadingWidgetText->SetText(FText::FromString(TEXT("곧 게임을 시작합니다.")));
		return;
	}

	LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), PlayerCount, MaxPlayerCount)));
}

void ULoadingWidget::SetMaximumPlayerNumber(const uint8& PlayerCount)
{
	MaxPlayerCount = PlayerCount;
}