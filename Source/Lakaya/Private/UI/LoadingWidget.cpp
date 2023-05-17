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

	//TODO: 텍스트 포맷을 따로 저장해두면 더 높은 퍼포먼스를 기대할 수 있습니다.
	LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), PlayerCount, MaxPlayerCount)));
}

void ULoadingWidget::SetMaximumPlayerNumber(const uint8& PlayerCount)
{
	MaxPlayerCount = PlayerCount;
}