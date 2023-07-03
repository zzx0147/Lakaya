#include "UI/LoadingWidget.h"

#include "GameMode/AIIndividualGameMode.h"
#include "GameMode/AIIndividualGameState.h"
#include "GameMode/OccupationGameState.h"
#include "Kismet/GameplayStatics.h"

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
	const auto CurrentGameState = UGameplayStatics::GetGameState(GetWorld());

	AOccupationGameState* NewOccupationGameState = Cast<AOccupationGameState>(CurrentGameState);
	AAIIndividualGameState* NewAIIndividualGameState = Cast<AAIIndividualGameState>(CurrentGameState);

	// Occupation Mode
	if (NewOccupationGameState)
	{
		if (PlayerCount == MaxPlayerCount)
		{
			LoadingWidgetText->SetText(FText::FromString(TEXT("loading . . .")));
			return;
		}

		LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), PlayerCount, MaxPlayerCount)));
	}

	// AIIndividual Mode
	if (NewAIIndividualGameState)
	{
		LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("loading. . ."))));
	}

	// TODO : Individual Mode 추가
}

void ULoadingWidget::SetMaximumPlayerNumber(const uint8& PlayerCount)
{
	MaxPlayerCount = PlayerCount;
}