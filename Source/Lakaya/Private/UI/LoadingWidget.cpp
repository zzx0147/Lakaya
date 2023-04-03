#include "UI/LoadingWidget.h"

#include "Character/MenuCallingPlayerController.h"
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
    IndividualGameState->OnChangeGameState.AddUObject(this, &ULoadingWidget::ReMoveLoadingWidget);
}

void ULoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULoadingWidget::OnChangeJoinedPlayers(int32 JoinedPlayers, int32 MaxPlayer)
{
    if (JoinedPlayers == MaxPlayer)
    {
        LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("곧 게임을 시작합니다."))));
        return;
    }
    
    LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), JoinedPlayers, MaxPlayer)));
}

void ULoadingWidget::ReMoveLoadingWidget(EGameState ChangeGamState)
{
    if (ChangeGamState == EGameState::Progress)
    {
        this->RemoveFromParent();
    }
}

