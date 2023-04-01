#include "UI/LoadingWidget.h"
#include "GameMode/IndividualGameMode.h"
#include "GameMode/IndividualGameState.h"
#include "Net/UnrealNetwork.h"

void ULoadingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
    if (IndividualGameState == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
        return;
    }

    OnChangeJoinedPlayers(IndividualGameState->NumPlayers, IndividualGameState->GetMaxPlayers());

    // 바인딩
    LoadingWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadingWidgetText")));
    if (LoadingWidgetText == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
        return;
    }

    IndividualGameState->OnChangeJoinedPlayers.AddUObject(this, &ULoadingWidget::OnChangeJoinedPlayers);
}

void ULoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULoadingWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void ULoadingWidget::OnChangeJoinedPlayers(int32 JoinedPlayers, int32 MaxPlayer)
{
    LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), JoinedPlayers, MaxPlayer)));
}