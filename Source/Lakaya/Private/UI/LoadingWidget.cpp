#include "UI/LoadingWidget.h"

#include "PlayerController/MenuCallingPlayerController.h"
#include "Net/UnrealNetwork.h"

void ULoadingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
    if (OccupationGameState == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
        return;
    }

    OnChangeJoinedPlayers(OccupationGameState->GetNumPlayers());
    
    // 바인딩
    LoadingWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadingWidgetText")));
    if (LoadingWidgetText == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
        return;
    }

    OccupationGameState->OnOccupationChangeJoinedPlayers.AddUObject(this, &ULoadingWidget::OnChangeJoinedPlayers);
    OccupationGameState->OnOccupationChangeGameState.AddUObject(this, &ULoadingWidget::ReMoveLoadingWidget);
}

void ULoadingWidget::OnChangeJoinedPlayers(uint8 JoinedPlayers)
{
    if (JoinedPlayers == OccupationGameState->GetMaxPlayers())
    {
        LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("곧 게임을 시작합니다."))));
        return;
    }
    
    LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), JoinedPlayers, OccupationGameState->GetMaxPlayers())));
}

void ULoadingWidget::ReMoveLoadingWidget(EOccupationGameState ChangeGamState)
{
    if (ChangeGamState == EOccupationGameState::Progress)
    {
        this->RemoveFromParent();
    }
}